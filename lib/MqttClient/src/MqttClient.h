#ifndef MqttClient_h
#define MqttClient_h
#include <WiFi.h>

class MqttClient
{
public:
    MqttClient(const char *serverUrl, uint16_t port, Client *wifiClient);
    ~MqttClient();

private:
    const char *_serverUrl;
    uint16_t _port;
    Client *_wifiClient;
};

#endif