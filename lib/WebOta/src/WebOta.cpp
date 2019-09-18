#include "WebOta.h"
#include <HTTPClient.h>
#include <cJSON.h>

HTTPClient *_phttp = NULL;

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

    int httpCode = _phttp->GET();
    if (httpCode > 0)
    {
        cJSON *firmwareMetadata = cJSON_Parse((char *)_phttp->getString().c_str());
        if (firmwareMetadata != NULL)
        {
            char *version = cJSON_GetObjectItemCaseSensitive(firmwareMetadata, "version")->valuestring;
            updateAvailable = (isLaterVersion(version, _currentVersion));
        }
    }
    else
    {
        Serial.println("Error on HTTP request");
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