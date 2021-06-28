#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <FastLED.h>
#include <FastLED_NeoMatrix.h>
#include <Fonts/TomThumb.h>

class Matrix
{
    private:
        int type;
        int tempCorrection;

        #define NUMBER_LED = 256;
        #ifdef ESP8266
            #define MATRIX_PIN = D2;
        #else
            #define MATRIX_PIN = 15;
        #endif
        

        enum MsgType {
            MsgType_Wifi,
            MsgType_Host,
            MsgType_Temp,
            MsgType_Audio,
            MsgType_Gest,
            MsgType_LDR,
            MsgType_Other
        };

        CRGB leds[256];
        FastLED_NeoMatrix *matrix;

        void hardwareAnimatedUncheck(int typ, int x, int y);
        void hardwareAnimatedCheck(MsgType typ, int x, int y);
    
    public:
        void init(int type, int tempCorrection);
        void setTempCorrection(int tempCorrection);
        void setType(int type);
        void hardwareCheck();       
        void serverSearch(int rounds, int typ, int x, int y);
        void hardwareAnimatedSearch(int typ, int x, int y);
        void setTextToMatrix(bool clear, byte red, byte green, byte blue, int xPos, int yPos, String text);
};