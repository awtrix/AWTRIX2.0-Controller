#include <Arduino.h>
#include <Wire.h>
#include <BME280_t.h>
#include "Adafruit_HTU21DF.h"
#include <Adafruit_BMP280.h>

class Temperature
{
    private:
        BME280<> BMESensor;
        Adafruit_BMP280 BMPSensor;
        Adafruit_HTU21DF htu = Adafruit_HTU21DF();

        int TEMPSENSOR_NONE = 0;
        int TEMPSENSOR_BME280 = 1;
        int TEMPSENSOR_HTU21D = 2;
        int TEMPSENSOR_BMP280 = 3;

        int currentTempSensor = TEMPSENSOR_NONE;

        float temp = 0.0;
        float humidity = 0.0;
        float presure = 0.0;

        bool isInit = false;
    public:
        
        int init();
        void updateValues();
        float getTemperature();
        float getHumidity();
        float getPresure();
        void printReadings();
    
    #ifdef ESP8266
        #define I2C_SDA D3
        #define I2C_SCL D1 
    #else
        #define I2C_SDA 21
        #define I2C_SCL 22 
    #endif
};