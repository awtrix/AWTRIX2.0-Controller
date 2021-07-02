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
        void flashProgress(unsigned int progress, unsigned int total);
        void fillScreen(byte red, byte green, byte blue);
        void drawPixel(uint16_t x_coordinate, uint16_t y_coordinate, uint16_t data);
        void drawPixel(uint16_t x_coordinate, uint16_t y_coordinate, byte red, byte green, byte blue);
        void clear();
        void setBrightness(byte brightness);
        void setCursor(uint16_t x, uint16_t y);
        uint16_t getCursorX();
        uint16_t getCursorY();
        void print(String text);
        void setTextColor(byte red, byte green, byte blue);
        void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, byte red, byte green, byte blue);
        void drawRect(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, byte red, byte green, byte blue);
        void fillRect(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, byte red, byte green, byte blue);
        void drawCircle(uint16_t x0, uint16_t y0, uint16_t radius, byte red, byte green, byte blue);
        void fillCircle(uint16_t x0, uint16_t y0, uint16_t radius, byte red, byte green, byte blue);
            
    private:
        uint32_t Wheel(byte WheelPos, int pos);
};