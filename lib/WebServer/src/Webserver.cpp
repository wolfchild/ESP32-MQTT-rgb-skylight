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

// ---- website handlers
void handleWifiConfigGet(AsyncWebServerRequest *request)
{
    Preferences prefs;
    WiFiConfig wifiConfig;

    prefs.begin("wifi");
    uint8_t a = prefs.getBytes("staCredentials", &wifiConfig, sizeof(wifiConfig));
    prefs.end();

    String json = "{";
    if (a == sizeof(WiFiConfig))
    {
        json += "\"ssid\":\"" + String(wifiConfig.ssid) + "\",";
        json += "\"key\":\"" + String(wifiConfig.password) + "\"";
    }
    json += "}";

    AsyncWebServerResponse *response = request->beginResponse(200, F(CONTENT_TYPE_JSON), json);
    response->addHeader(F(CORS_HEADER), "*");
    request->send(response);
}

void handleWifiConfigPost(AsyncWebServerRequest *request)
{
    String json = "{";
    int responseCode = 500;

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

        json += "\"message\":\"Configuration updated successfully.";
        responseCode = 200;
    }
    else
    {
        json += "\"message\":\"Error while saving configuration.";
        responseCode = 500;
    }
    json += "}";

    AsyncWebServerResponse *response = request->beginResponse(responseCode, F(CONTENT_TYPE_JSON), json);
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

    // API POST
    server.on("/api/wificonfig", HTTP_POST, handleWifiConfigPost);
    // Start server
    server.begin();
}

WebServer::~WebServer()
{
    server.end();
}
