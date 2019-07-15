
// demonstrate the use of WiFi.mode(SHUTDOWN/RESUME)
// released to public domain

// current on wemos d1 mini (including: ldo, usbserial chip):
// ~85mA during normal operations
// ~30mA during wifi shutdown
//  ~5mA during deepsleep

#ifndef STASSID
#define STASSID "mynetwork"
#define STAPSK "mynetworkpasswd"
#endif

#define WAIT_NTP 0             // define this to 1 for NTP check too

#include <ESP8266WiFi.h>
#include <coredecls.h>         // crc32()
#include <include/WiFiState.h> // WiFiState structure details

enum state_e {
  e_initial,
  e_start_resume,
  e_start_normal,
  e_off_restart,
  e_wait_connected,
  e_wait_ntp,
  e_shutdown,
  e_wait_shutdown
};

static state_e step = e_initial; // step
static int wifi_timeout = 0; // wifi timeout counter
static bool time_is_set = false; // WAIT_NTP=1: wait for network - dhcp packet must have ntp server

// non volatile data
struct nv_s {
  WiFiState wss; // core's wifi save state

  uint32_t crc;
  struct {
    int rstcounter[7];
  } data;
};
static nv_s* nv = (nv_s*)RTC_USER_MEM; // user non volatile area

#define SEP "###### "
#define EV  "!!!!!! "
#define NFO "------ "

void resetUserCrc() {
  nv->crc = crc32(&nv->data, sizeof(nv->data));
}

void printNv() {
  Serial.printf(NFO "nfo1/2 wifi-nv-state: valid=%d, "
                "persistent=%d, "
                "mode=%d, "
                "channel=%d, "
                "ip=%s, "
                "dns=%s, "
                "ntp=%s\n",
                WiFi.shutdownValidCRC(&nv->wss),
                nv->wss.state.persistent,
                nv->wss.state.mode,
                nv->wss.state.channel,
                IPAddress(&nv->wss.state.ip.ip).toString().c_str(),
                IPAddress(&nv->wss.state.dns[0]).toString().c_str(),
                IPAddress(&nv->wss.state.ntp[0]).toString().c_str());

  Serial.printf(NFO "nfo2/2 rst reason counters: default:%d wdt:%d exception:%d softwdt:%d reset:%d deepsleep:%d extsys:%d\n",
                nv->data.rstcounter[0],
                nv->data.rstcounter[1],
                nv->data.rstcounter[2],
                nv->data.rstcounter[3],
                nv->data.rstcounter[4],
                nv->data.rstcounter[5],
                nv->data.rstcounter[6]);
}

void timeset_cb() {
  time_is_set = true;

  static bool first = true;
  if (first) {
    first = false;
  }
}

decltype(millis()) startup;


WiFiEventHandler evOff = WiFi.onWiFiModeChange([](const WiFiEventModeChange& event) {
  Serial.printf(EV "mode changed event: ev:%d->%d getMode=%d\n", event.oldMode, event.newMode, wifi_get_opmode());
});

void preinit() {
  ESP8266WiFiClass::preinitWiFiOff();
}

void setup() {
  WiFi.persistent(false);
  startup = millis();
  Serial.begin(115200);
  settimeofday_cb(timeset_cb);

  // prepare non volatile user structure
  if (crc32(&nv->data, sizeof(nv->data)) != nv->crc) {
    memset(&nv->data, 0, sizeof(nv->data));
    Serial.printf(SEP "reset NV user data\n");
  }
  // update reset reason
  nv->data.rstcounter[system_get_rst_info()->reason]++;
  // recalculate crc
  resetUserCrc();
  // nfo
  printNv();

  Serial.println("setup()");
}

#define TEST(x...) ({ auto v = x; Serial.printf(SEP "'%s': result = %d\n", #x, v); v; })

void loop() {

  static int prev = 255;
  if (step != prev) {
    prev = step;
    Serial.printf(NFO "step %d - wifi getMode=%d=%d  heap=%d freeheap=%d\n",
                  prev,
                  WiFi.getMode(),
                  wifi_get_opmode(),
                  ESP.getFreeHeap(),
                  ESP.getFreeHeap());
    printNv();
  }

  switch (step) {
    case e_initial: {
        if (WiFi.shutdownValidCRC(&nv->wss)) {
          step = e_start_resume;
        } else {
          step = e_start_normal;
        }
        break;
      }


    case e_start_resume:
      Serial.println(SEP "CRC valid => WIFI_RESUME");
      startup = millis();

      if (!TEST(WiFi.mode(WIFI_RESUME, &nv->wss))) {
        Serial.printf(SEP "issue resuming WiFi\n");
        step = e_off_restart;
      } else {
        Serial.printf(SEP "waiting for connected\\n");
        step = e_wait_connected;
      }
      break;


    case e_start_normal:
      Serial.printf(SEP "CRC NOT valid, begin/WIFI_STA (current mode = %d)\n", wifi_get_opmode());
      startup = millis();
      if (!TEST(WiFi.mode(WIFI_STA)) || !TEST(WiFi.begin(STASSID, STAPSK))) {
        Serial.printf(SEP "issue setting up STA\n");
        step = e_off_restart;
      } else {
        Serial.printf(SEP "waiting for connected\n");
        step = e_wait_connected;
      }
      break;


    case e_wait_connected:
      if (WiFi.status() == WL_CONNECTED) {
        Serial.printf(SEP "connected! ---- startup time: %ld ms ----\n\n\n", millis() - startup);
        wifi_timeout = 0;
        if (WAIT_NTP) {
          step = e_wait_ntp;
          Serial.printf(SEP "wait for NTP\n");
        } else {
          step = e_shutdown;
        }
      } else if ((millis() - startup > 10000)) {
        Serial.printf(SEP "connected TIMEOUT! status=%d\n", WiFi.status());
        wifi_timeout++;
        step = e_off_restart;
      }
      break;


    case e_off_restart:
      Serial.printf(SEP "OFF -> wait 2s\n");
      (void)TEST(WiFi.mode(WIFI_OFF));
      delay(2000); // test - mad wifi loop until :oom if delay not there - to verify
      step = e_initial;
      break;


    case e_wait_ntp:
      // check when NTP has set time
      if (time_is_set) {
        Serial.printf(SEP "NTP is set\n");
        time_is_set = false;
        step = e_shutdown;
      }
      break;


    case e_shutdown: {
        static int deepsleep = 0;
        switch (++deepsleep) {
          case 1: {
              Serial.println(SEP "WIFI_OFF for 5s");
              TEST(WiFi.mode(WIFI_OFF, &nv->wss));
              break;
            }
          case 2: // several loop on shutdown
          case 3: // to check if it affects
          case 4: { // reconnection duration
              Serial.println(SEP "WIFI_SHUTDOWN for 5s");
              TEST(WiFi.mode(WIFI_SHUTDOWN, &nv->wss));
              break;
            }
          default: {
              Serial.println(SEP "DEEPSLEEP for 5s (bind GPIO16 <=> RST)");
              TEST(WiFi.mode(WIFI_SHUTDOWN, &nv->wss));
              Serial.flush();
              ESP.deepSleep(5000000);
              // will reboot, GPIO16 must be connected to reset
            }
        }

        startup = millis();
        step = e_wait_shutdown;
        break;
      }


    case e_wait_shutdown:
      if (millis() - startup > 5000) {
        step = e_start_resume;
      }
      break;
  }
}
