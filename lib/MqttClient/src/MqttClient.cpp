#include "MqttClient.h"
#include "WiFi.h"

#include <PubSubClient.h>

PubSubClient *_pMqttClient = NULL;



// Callback function

void callback(char* topic, byte* payload, unsigned int length) {

  // In order to republish this payload, a copy must be made
  // as the orignal payload buffer will be overwritten whilst
  // constructing the PUBLISH packet.

  // Allocate the correct amount of memory for the payload copy
  byte* p = (byte*)malloc(length);

  // Copy the payload to the new buffer
  memcpy(p,payload,length);

  _pMqttClient->publish("outTopic", p, length);

  // Free the memory
  free(p);
}



MqttClient::MqttClient(const char *serverUrl, uint16_t port, Client wifiClient)
{
    _serverUrl = serverUrl;
    _port = port;
    _wifiClient = &wifiClient;

    _pMqttClient = new PubSubClient(_serverUrl, port, callback, wifiClient);
}

MqttClient::~MqttClient()
{
}


