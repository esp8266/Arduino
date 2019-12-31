#ifndef WIFI_HEALTH_H
#define WIFI_HEALTH_H

bool printLocalTime(Print& oPrint, time_t timeIn = 0, const char *fmt = "%I:%M:%S %p %Z, %a %b %d, %Y");
bool printLocalTime(Print& oPrint, time_t timeIn, const char *fmt);
extern bool WifiUp;

//static unsigned long last_garp = 0;

typedef struct _WIFI_HEALTH {
  int32_t connected_count;
  int32_t rssi_sum;
  uint32_t rssi_count;
  time_t gtime_adjust;
  time_t connected_time;
  time_t disconnected_time;
  time_t uptime_sum;
  time_t uptime_max;
  time_t uptime_min;
  uint8  bssid[6];
  uint8_t channel;
  int8_t rssi_max;
  int8_t rssi_min;
  int8_t rssi;
} wifi_health_t;
extern wifi_health_t wifiHealth;


#ifndef MAX_CONNECTION_LOST_TIME_LOG
#define MAX_CONNECTION_LOST_TIME_LOG (8)
#endif
typedef struct _WIFI_DISCONNECT_LOG {
  time_t time;
  uint8_t channel;
  uint8_t reason;
  int8_t rssi;
} WiFiDisconnectLog_t;
extern WiFiDisconnectLog_t wifi_disconnect_log[MAX_CONNECTION_LOST_TIME_LOG];

// void updateWiFiStats(void);
// bool printWiFiStats(Print& oStream);
// void onWiFiConnected(WiFiEventStationModeConnected data);
// void onWiFiDisconnected(WiFiEventStationModeDisconnected data);


extern Ticker scheduleCheck;

#endif
