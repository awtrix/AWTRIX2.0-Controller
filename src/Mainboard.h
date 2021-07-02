#include <SPI.h>
#include <Arduino.h>
#include <HardwareSerial.h>

class Mainboard
{
    #ifdef ESP8266
        #define LDR_PIN A0

        int BUTTON_LEFT_PIN = D0;
        int BUTTON_MIDD_PIN = D4;
        int BUTTON_RIGHT_PIN = D8;

        int MATRIX_PIN = D2; 

        int DFP_TX_PIN = D7; 
        int DFP_RX_PIN = D5; 
    #else
        #define LDR_PIN 4

        int STATUS_LED_PIN = 5;

        int BUTTON_LEFT_PIN = 12;
        int BUTTON_MIDD_PIN = 13;
        int BUTTON_RIGHT_PIN = 14;

        int MATRIX_PIN = 15; 

        int DFP_TX_PIN = 16; 
        int DFP_RX_PIN = 17; 
    #endif

    private:
      
    public:
        bool ledState = false;
        void init();
        void controlLED(bool LEDstate);
};