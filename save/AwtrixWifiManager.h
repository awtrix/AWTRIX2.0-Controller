#include <Arduino.h>
#ifdef ESP8266
    #include <ESP8266WiFi.h>
#else
    #include <WiFi.h>
#endif

#include <WiFiManager.h>

class AwtrixWifiManager
{
    private:
    

    public:
    void init(char* awtrixServer, char* port);
    void static configModeCallback(WiFiManager *myWiFiManager);
    void static saveConfigCallback();
    String getIpAddress();
    bool getShouldSaveConfig();
    char* getPort();
};