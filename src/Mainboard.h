#include <Arduino.h>
class Mainboard
{
    private:

    public:
        void setup();
        void controlLED(bool LEDstate);
        void setupMatrix(int type, int tempCorrection);
        void setTextToMatrix(bool *clear, byte *red, byte *green, byte *blue, int *xPos, int *yPos, String *text);
    
    #ifdef ESP8266
        int BUTTON_LEFT_PIN = D0;
        int BUTTON_MIDD_PIN = D4;
        int BUTTON_RIGHT_PIN = D8;

        int MATRIX_PIN = D2; 

        int DFP_TX_PIN = D7; 
        int DFP_RX_PIN = D5; 
    #else
        int STATUS_LED_PIN = 5;

        int BUTTON_LEFT_PIN = 12;
        int BUTTON_MIDD_PIN = 13;
        int BUTTON_RIGHT_PIN = 14;

        int MATRIX_PIN = 15; 

        int DFP_TX_PIN = 16; 
        int DFP_RX_PIN = 17; 
    #endif
};