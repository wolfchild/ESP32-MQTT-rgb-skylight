#include "WebServer.h"
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <SPIFFS.h>
#include <Wifi.h>
#include <WifiConfig.h>

#define CORS_HEADER "Access-Control-Allow-Origin"
#define CONTENT_TYPE_JSON "application/json"
#define CONTENT_TYPE_PLAIN "text/plain"
#define CONTENT_TYPE_HTML "text/html"

#define WIFI_GET_WIFI_SSID_TEMPLATE "{\"ssid\":\"%s\",\"bssid\":\"%s\",\"rssi\":%d,\"channel\":%d,\"security\":%d}"
#define WIFI_GET_STATUS_TEMPLATE "{\"freeheap\":%d, \"minfreeheap\":%d, \"maxallocheap\":%d}"

#define WIFI_GET_CONFIG_SUCCESS_TEMPLATE "{\"ssid\":\"%s\",\"key\":\"%s\"}"
#define WIFI_GET_CONFIG_ERROR_TEMPLATE "{\"message\":\"Error while retrieving configuration.\"}"

#define WIFI_POST_CONFIG_SUCCESS_TEMPLATE "{\"message\":\"Configuration updated successfully\"}"
#define WIFI_POST_CONFIG_ERROR_TEMPLATE "{\"message\":\"Error while saving configuration.\"}"

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// ---- API handlers
void handleWifiScan(AsyncWebServerRequest *request)
{
    int n = WiFi.scanNetworks();

    String json = "[";
    if (n > 0)
    { // scan finished
        for (int i = 0; i < n; ++i)
        {
            if (i)
                json += ",";
            json += "{";
            json += "\"rssi\":" + String(WiFi.RSSI(i));
            json += ",\"ssid\":\"" + WiFi.SSID(i) + "\"";
            json += ",\"bssid\":\"" + WiFi.BSSIDstr(i) + "\"";
            json += ",\"channel\":" + String(WiFi.channel(i));
            json += ",\"security\":" + String(WiFi.encryptionType(i));
            json += "}";
        }
        // save scan result memory
        WiFi.scanDelete();
    }
    json += "]";

    AsyncWebServerResponse *response = request->beginResponse(200, F(CONTENT_TYPE_JSON), json);
    response->addHeader(F(CORS_HEADER), "*");
    request->send(response);
}

void handleWifiConfigGet(AsyncWebServerRequest *request)
{
    Preferences prefs;
    WiFiConfig wifiConfig;

    int responseCode = 500;
    char *responseBuffer;

   //digitalWrite(2, !digitalRead(2));

    prefs.begin("wifi");
    uint8_t a = prefs.getBytes("staCredentials", &wifiConfig, sizeof(wifiConfig));
    prefs.end();

    if (a == sizeof(WiFiConfig))
    {
        uint8_t bufferSize = sizeof(WIFI_GET_CONFIG_SUCCESS_TEMPLATE) - 4 + sizeof(wifiConfig.ssid) + sizeof(wifiConfig.password) + 2;
        responseBuffer = (char *)malloc(bufferSize);

        sprintf_P(responseBuffer, WIFI_GET_CONFIG_SUCCESS_TEMPLATE, wifiConfig.ssid, "********\0");

        responseCode = 200;
    }
    else
    {
        uint8_t bufferSize = sizeof(WIFI_GET_CONFIG_ERROR_TEMPLATE) + 1;
        responseBuffer = (char *)malloc(bufferSize);
        responseCode = 500;
    }

    AsyncWebServerResponse *response = request->beginResponse(responseCode, F(CONTENT_TYPE_JSON), responseBuffer);
    response->addHeader(F(CORS_HEADER), "*");
    request->send(response);

    free(responseBuffer);
}

void handleWifiConfigPost(AsyncWebServerRequest *request)
{
    int responseCode = 500;
    char *responseBuffer;

    if (request->hasParam("ssid", true) && request->hasParam("key", true))
    {
        AsyncWebParameter *p;
        Preferences prefs;
        WiFiConfig wifiConfig;
        prefs.begin("wifi");

        p = request->getParam("ssid", true);
        strlcpy(wifiConfig.ssid, p->value().c_str(), sizeof(wifiConfig.ssid));

        p = request->getParam("key", true);
        strlcpy(wifiConfig.password, p->value().c_str(), sizeof(wifiConfig.password));

        prefs.putBytes("staCredentials", &wifiConfig, sizeof(wifiConfig));
        prefs.end();

        responseBuffer = (char *)malloc(sizeof(WIFI_POST_CONFIG_SUCCESS_TEMPLATE) + 1);
        strlcpy(responseBuffer, WIFI_POST_CONFIG_SUCCESS_TEMPLATE, sizeof(WIFI_POST_CONFIG_SUCCESS_TEMPLATE));
        responseCode = 200;
    }
    else
    {
        responseBuffer = (char *)malloc(sizeof(WIFI_POST_CONFIG_ERROR_TEMPLATE) + 1);
        strlcpy(responseBuffer, WIFI_POST_CONFIG_ERROR_TEMPLATE, sizeof(WIFI_POST_CONFIG_ERROR_TEMPLATE));
        responseCode = 500;
    }

    AsyncWebServerResponse *response = request->beginResponse(responseCode, F(CONTENT_TYPE_JSON), responseBuffer);
    response->addHeader(F(CORS_HEADER), "*");
    request->send(response);

    free(responseBuffer);
}

void handleStatusPage(AsyncWebServerRequest *request)
{
    //char *responseBuffer;
    Serial.println(ESP.getFreeHeap());
    //Serial.println(ESP.getHeapSize());
    //Serial.println(ESP.getMaxAllocHeap());
    //Serial.println(ESP.getMinFreeHeap());

    AsyncWebServerResponse *response = request->beginResponse(200, F(CONTENT_TYPE_JSON), "{}");
    response->addHeader(F(CORS_HEADER), "*");
    request->send(response);
}

WebServer::WebServer()
{
    // Initialize SPIFFS
    if (!SPIFFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
    server.serveStatic("/index.html", SPIFFS, "/index.html");
    server.serveStatic("/index.js", SPIFFS, "/index.js");
    server.serveStatic("/css/", SPIFFS, "/css/");
    server.serveStatic("/images/", SPIFFS, "/images/");

    // API GET
    server.on("/api/scan", HTTP_GET, handleWifiScan);
    server.on("/api/wificonfig", HTTP_GET, handleWifiConfigGet);
    server.on("/api/status", HTTP_GET, handleStatusPage);

    // API POST
    server.on("/api/wificonfig", HTTP_POST, handleWifiConfigPost);
    // Start server
    server.begin();
}

WebServer::~WebServer()
{
    server.end();
}
