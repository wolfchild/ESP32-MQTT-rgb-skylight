#include "WebOta.h"
#include <HTTPClient.h>
#include <cJSON.h>
#include <esp_https_ota.h>

HTTPClient *_phttp = NULL;
char *_firmwareUrl;

WebOta::WebOta(const char *metadataUrl, const char *currentVersion, const char *rootCertAuthority)
{
    _metadataUrl = metadataUrl;
    _currentVersion = currentVersion;
    _rootCertAuthority = rootCertAuthority;
}

bool WebOta::IsUpdateAvailable()
{
    bool updateAvailable = false;

    _phttp = new HTTPClient();
    _phttp->begin(_metadataUrl, _rootCertAuthority);

    if (_phttp->GET() == 200)
    {
        char *payload = (char *)_phttp->getString().c_str();

        cJSON *firmwareMetadata = cJSON_Parse(payload);
        if (firmwareMetadata != NULL)
        {
            char *version = cJSON_GetObjectItemCaseSensitive(firmwareMetadata, "version")->valuestring;
            _firmwareUrl = cJSON_GetObjectItemCaseSensitive(firmwareMetadata, "url")->valuestring;
            updateAvailable = (isLaterVersion(version, _currentVersion));
        }
    }
    _phttp->end();
    return updateAvailable;
}

WebOta::~WebOta()
{
    delete _phttp;
}

bool WebOta::UpdateFirmware()
{
    esp_http_client_config_t ota_client_config;

    ota_client_config.url = _firmwareUrl;
    ota_client_config.cert_pem = _rootCertAuthority;

    esp_err_t ret = esp_https_ota(&ota_client_config);
    if (ret == ESP_OK)
    {
        esp_restart();
    }
    return false;
}

bool WebOta::isLaterVersion(char *serverVersion, const char *currentVersion)
{
    int currentMajor = 0, currentMinor = 0, currentRevision = 0;
    int serverMajor = 0, serverMinor = 0, serverRevision = 0;

    sscanf(serverVersion, "%d.%d.%d", &serverMajor, &serverMinor, &serverRevision);
    sscanf(currentVersion, "%d.%d.%d", &currentMajor, &currentMinor, &currentRevision);

    return (serverMajor > currentMajor) ||
           (serverMinor > currentMinor) ||
           (serverRevision > currentRevision);
}