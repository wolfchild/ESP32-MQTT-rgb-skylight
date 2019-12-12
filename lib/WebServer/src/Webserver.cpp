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

// Replaces placeholder with LED state value
String processor(const String &var)
{
    Serial.println(var);
    if (var == "STATE")
    {
        return "OFF";
    }
    return String();
}

// ---- API handlers
void handleWifiScan(AsyncWebServerRequest *request)
{
    //int n = WiFi.scanComplete();
    //if (n == WIFI_SCAN_FAILED)
    //{
    int n = WiFi.scanNetworks();
    //}
    Serial.printf("scanning wifi (%d)\n", n);

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
        json += "\"password\":\"" + String(wifiConfig.password) + "\"";
    }
    json += "}";

    AsyncWebServerResponse *response = request->beginResponse(200, F(CONTENT_TYPE_JSON), json);
    response->addHeader(F(CORS_HEADER), "*");
    request->send(response);
}

void handleWifiConfigPost()
{
}

WebServer::WebServer()
{
    // Initialize SPIFFS
    if (!SPIFFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    server.serveStatic("/index.html", SPIFFS, "/index.html");
    server.serveStatic("/index.js", SPIFFS, "/index.js");
    server.serveStatic("/css/", SPIFFS, "/css/");
    server.serveStatic("/images/", SPIFFS, "/images/");

    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html", String(), false, processor);
    });

    // API get
    server.on("/api/scan", HTTP_GET, handleWifiScan);
    server.on("/api/wificonfig", HTTP_GET, handleWifiConfigGet);

    // Start server
    server.begin();
}

WebServer::~WebServer()
{
    server.end();
}
