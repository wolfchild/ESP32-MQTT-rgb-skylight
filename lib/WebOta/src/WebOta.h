#ifndef WebOta_h
#define WebOta_h

class WebOta
{
public:
    WebOta(const char *metadataUrl, const char *currentVersion, const char *rootCertAuthority);
    ~WebOta();
    bool IsUpdateAvailable();
    bool UpdateFirmware();

private:
    const char *_metadataUrl;
    const char *_currentVersion;
    const char *_rootCertAuthority;
    bool _updateAvailable = false;
    bool isLaterVersion(char *serverVersion, const char *currentVersion);
};

#endif
