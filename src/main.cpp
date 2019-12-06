#include <Arduino.h>
#include <ESPmDNS.h>
#include <Ticker.h>
#include <WiFi.h>

#include <WebOta.h>
#include <WebServer.h>
#include <MqttClient.h>

IPAddress ipAddress;
WebOta *webOta;
WebServer *webServer;
Ticker updateChecker;
const char *ssid = "*****";
const char *password = "*****";

const char *ownssid = "skylight";
const char *ownpassword = "skylight-psw";

const float updateCheckInterval = 86400; // every 24 hours

const char *otaMetadataUrl = "https://raw.githubusercontent.com/wolfchild/ESP32-MQTT-rgb-skylight/master/current-release.json";
const char *firmwareVersion = "0.0.1";
const char *otaRootCertAuthority =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs\n"
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
    "d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n"
    "ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL\n"
    "MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n"
    "LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug\n"
    "RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm\n"
    "+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW\n"
    "PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM\n"
    "xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB\n"
    "Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3\n"
    "hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg\n"
    "EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF\n"
    "MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA\n"
    "FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec\n"
    "nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z\n"
    "eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF\n"
    "hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2\n"
    "Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\n"
    "vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep\n"
    "+OkuE6N36B9K\n"
    "-----END CERTIFICATE-----\n";

IPAddress setupWiFiConnection(const char *ssid, const char *password)
{
  uint8_t maxRetries = 20;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while ((WiFi.status() != WL_CONNECTED) && (--maxRetries != 0))
  { // Wait for the Wi-Fi to connect
    delay(500);
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    int n = WiFi.scanNetworks();
    if (n != 0)
    {
      for (int i = 0; i < n; i++)
      {
        Serial.print(i);
      }
    }
    WiFi.softAP(ownssid, ownpassword);

    Serial.println("Created new network successfully.");
    return WiFi.softAPIP();
  }

  Serial.println("Connection to existing network established.");
  return WiFi.localIP();
}

bool startmDNSResponder(const char *hostName)
{
  Serial.println("Starting mDNS responder");
  if (MDNS.begin(hostName))
  {
    Serial.println("Adding mDNS services");
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("mqtt", "tcp", 1883);
    return (true);
  }

  return (false);
}

void processFirmwareUpdate()
{
  webOta = new WebOta(otaMetadataUrl, firmwareVersion, otaRootCertAuthority);
  if (webOta->IsUpdateAvailable())
  {
    Serial.println("Update available");
    webOta->UpdateFirmware();
    Serial.println("Firmware update in progress");
  }
  else
  {
    Serial.println("No update due");
  }
  delete webOta;
}

void setup()
{
  Serial.begin(115200);

  // Establish WiFi connectivitiy
  ipAddress = setupWiFiConnection(ssid, password);

  Serial.print("IP address: ");
  Serial.println(ipAddress);

  // perform OTA update check
  processFirmwareUpdate();

  // initialize firmware OTA update checker
  updateChecker.attach(updateCheckInterval, processFirmwareUpdate);

  // start admin web server
  webServer = new WebServer();

  // start MQTT client

  // Start mDNS responder
  startmDNSResponder("skylight");
}

void loop()
{
  // put your main code here, to run repeatedly:
}
