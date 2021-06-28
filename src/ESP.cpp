#include <Arduino.h>
class ESPControl {
    public:
    /// This function performs a software reset of the esp
    void reset(){
        #ifdef ESP8266
            ESP.reset();
        #else
            ESP.restart();
        #endif
    }

    /// This function returns the chip ID of the esp. 
    /// Note: Does not currently work with the ESP32.
    String getChipID(){
        #ifdef ESP8266
            return GetChipID;
        #else
            return "";
        #endif
    }
};