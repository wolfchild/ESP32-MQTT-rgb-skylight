#ifndef WiFiConfig_h
#define WiFiConfig_h

typedef struct wificonfig
{
    char ssid[32];
    char password[63];
} WiFiConfig;

#endif // WiFiConfig_h