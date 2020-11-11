#ifndef WIFI_HEALTH_H
#define WIFI_HEALTH_H

bool printLocalTime(Print& oPrint, time_t timeIn = 0, const char *fmt = "%I:%M:%S %p %Z, %a %b %d, %Y");
bool printLocalTime(Print& oPrint, time_t timeIn, const char *fmt);
extern bool WifiUp;

typedef struct _WIFI_HEALTH {
  int32_t connected_count = 0;
  int32_t rssi_sum = 0;
  uint32_t rssi_count = 0;
  time_t gtime_adjust = 0;
  time_t connected_time = 0;
  time_t uptime_sum = 0;
  time_t uptime_max = 0;
  time_t uptime_min = (time_t)LONG_MAX;
  time_t disconnected_time = 0;
  time_t downtime_sum = 0;
  time_t downtime_max = 0;
  time_t downtime_min = (time_t)LONG_MAX;
  uint8  bssid[6] = "";
  uint8_t channel = 0;
  int8_t rssi_max = INT8_MIN;
  int8_t rssi_min = INT8_MAX;
  int8_t rssi = 0;
} wifi_health_t;
extern wifi_health_t wifiHealth;


#ifndef MAX_CONNECTION_LOST_TIME_LOG
#define MAX_CONNECTION_LOST_TIME_LOG (8)
#endif
typedef struct _WIFI_DISCONNECT_LOG {
  time_t time = 0;
  uint8_t channel = 0;
  uint8_t reason = 0;
  int8_t rssi = 0;
} WiFiDisconnectLog_t;
extern WiFiDisconnectLog_t wifi_disconnect_log[MAX_CONNECTION_LOST_TIME_LOG];

// void updateWiFiStats(void);
// bool printWiFiStats(Print& oStream);
// void onWiFiConnected(WiFiEventStationModeConnected data);
// void onWiFiDisconnected(WiFiEventStationModeDisconnected data);


extern Ticker scheduleCheck;

#endif
