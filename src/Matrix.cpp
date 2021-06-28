#include "Matrix.h"

enum MsgType {
	MsgType_Wifi,
	MsgType_Host,
	MsgType_Temp,
	MsgType_Audio,
	MsgType_Gest,
	MsgType_LDR,
	MsgType_Other
};

void Matrix::init(int type, int tempCorrection, int PIN){
    PIN = PIN;
    type = type;
    tempCorrection = tempCorrection;

    setType(type);
	FastLED.addLeds<NEOPIXEL, 15>(leds, 256).setCorrection(TypicalLEDStrip);
    //setTempCorrection(tempCorrection, PIN, 256);
}

void Matrix::hardwareCheck(){

}

void Matrix::setTextToMatrix(bool clear, byte red, byte green, byte blue, int xPos, int yPos, String text){
  if(clear){
    matrix->clear();
  }
  matrix->setTextColor(matrix->Color(red, green, blue));
  matrix->setCursor(xPos, yPos);
  matrix->print(text);
  matrix->show();
}

void Matrix::hardwareAnimatedUncheck(int typ, int x, int y)
{
  int wifiCheckTime = millis();
  int wifiCheckPoints = 0;
  while (millis() - wifiCheckTime < 2000)
  {
    while (wifiCheckPoints < 10)
    {
      matrix->clear();
      switch (typ)
      {
      case 0:
        matrix->setCursor(7, 6);
        matrix->print("WiFi");
        break;
      case 1:
        matrix->setCursor(1, 6);
        matrix->print("Server");
        break;
      case 2:
        matrix->setCursor(7, 6);
        matrix->print("Temp");
        break;
      case 4:
        matrix->setCursor(3, 6);
        matrix->print("Gest.");
        break;
      }

      switch (wifiCheckPoints)
      {
      case 9:
        matrix->drawPixel(x, y + 4, matrix->Color(235, 64, 52));
      case 8:
        matrix->drawPixel(x - 1, y + 3, matrix->Color(235, 64, 52));
      case 7:
        matrix->drawPixel(x - 2, y + 2, matrix->Color(235, 64, 52));
      case 6:
        matrix->drawPixel(x - 3, y + 1, matrix->Color(235, 64, 52));
      case 5:
        matrix->drawPixel(x - 4, y, matrix->Color(235, 64, 52));
      case 4:
        matrix->drawPixel(x - 4, y + 4, matrix->Color(235, 64, 52));
      case 3:
        matrix->drawPixel(x - 3, y + 3, matrix->Color(235, 64, 52));
      case 2:
        matrix->drawPixel(x - 2, y + 2, matrix->Color(235, 64, 52));
      case 1:
        matrix->drawPixel(x - 1, y + 1, matrix->Color(235, 64, 52));
      case 0:
        matrix->drawPixel(x, y, matrix->Color(235, 64, 52));
        break;
      }
      wifiCheckPoints++;
      matrix->show();
      delay(100);
    }
  }
}

void Matrix::hardwareAnimatedCheck(MsgType typ, int x, int y)
{
  int wifiCheckTime = millis();
  int wifiCheckPoints = 0;
  while (millis() - wifiCheckTime < 2000)
  {
    while (wifiCheckPoints < 7)
    {
      matrix->clear();
      switch (typ)
      {
      case MsgType_Wifi:
        matrix->setCursor(7, 6);
        matrix->print("WiFi");
        break;
      case MsgType_Host:
        matrix->setCursor(5, 6);
        matrix->print("Host");
        break;
      case MsgType_Temp:
        matrix->setCursor(7, 6);
        matrix->print("Temp");
        break;
      case MsgType_Audio:
        matrix->setCursor(3, 6);
        matrix->print("Audio");
        break;
      case MsgType_Gest:
        matrix->setCursor(3, 6);
        matrix->print("Gest.");
        break;
      case MsgType_LDR:
        matrix->setCursor(7, 6);
        matrix->print("LDR");
        break;
      }

      switch (wifiCheckPoints)
      {
      case 6:
        matrix->drawPixel(x, y, matrix->Color(0, 7, 224));
      case 5:
        matrix->drawPixel(x - 1, y + 1, matrix->Color(0, 7, 224));
      case 4:
        matrix->drawPixel(x - 2, y + 2, matrix->Color(0, 7, 224));
      case 3:
        matrix->drawPixel(x - 3, y + 3, matrix->Color(0, 7, 224));
      case 2:
        matrix->drawPixel(x - 4, y + 4, matrix->Color(0, 7, 224));
      case 1:
        matrix->drawPixel(x - 5, y + 3, matrix->Color(0, 7, 224));
      case 0:
        matrix->drawPixel(x - 6, y + 2, matrix->Color(0, 7, 224));
        break;
      }
      wifiCheckPoints++;
      matrix->show();
      delay(100);
    }
  }
}

void Matrix::serverSearch(int rounds, int typ, int x, int y)
{
  matrix->clear();
  matrix->setTextColor(0xFFFF);
  matrix->setCursor(5, 6);
  matrix->print("Host");

if (typ == 0)
  {
    switch (rounds)
    {
    case 3:
      matrix->drawPixel(x, y, matrix->Color(0, 7, 224));
      matrix->drawPixel(x + 1, y + 1, matrix->Color(0, 7, 224));
      matrix->drawPixel(x + 2, y + 2, matrix->Color(0, 7, 224));
      matrix->drawPixel(x + 3, y + 3, matrix->Color(0, 7, 224));
      matrix->drawPixel(x + 2, y + 4, matrix->Color(0, 7, 224));
      matrix->drawPixel(x + 1, y + 5, matrix->Color(0, 7, 224));
      matrix->drawPixel(x, y + 6, matrix->Color(0, 7, 224));
    case 2:
      matrix->drawPixel(x - 1, y + 2, matrix->Color(0, 7, 224));
      matrix->drawPixel(x, y + 3, matrix->Color(0, 7, 224));
      matrix->drawPixel(x - 1, y + 4, matrix->Color(0, 7, 224));
    case 1:
      matrix->drawPixel(x - 3, y + 3, matrix->Color(0, 7, 224));
    case 0:
      break;
    }
  }
  else if (typ == 1)
  {

    switch (rounds)
    {
    case 12:
      //matrix->drawPixel(x+3, y+2, 0x22ff);
      matrix->drawPixel(x + 3, y + 3, matrix->Color(0, 7, 224));
      //matrix->drawPixel(x+3, y+4, 0x22ff);
      matrix->drawPixel(x + 3, y + 5, matrix->Color(0, 7, 224));
      //matrix->drawPixel(x+3, y+6, 0x22ff);
    case 11:
      matrix->drawPixel(x + 2, y + 2, matrix->Color(0, 7, 224));
      matrix->drawPixel(x + 2, y + 3, matrix->Color(0, 7, 224));
      matrix->drawPixel(x + 2, y + 4, matrix->Color(0, 7, 224));
      matrix->drawPixel(x + 2, y + 5, matrix->Color(0, 7, 224));
      matrix->drawPixel(x + 2, y + 6, matrix->Color(0, 7, 224));
    case 10:
      matrix->drawPixel(x + 1, y + 3, matrix->Color(0, 7, 224));
      matrix->drawPixel(x + 1, y + 4, matrix->Color(0, 7, 224));
      matrix->drawPixel(x + 1, y + 5, matrix->Color(0, 7, 224));
    case 9:
      matrix->drawPixel(x, y + 4, matrix->Color(0, 7, 224));
    case 8:
      matrix->drawPixel(x - 1, y + 4, matrix->Color(0, 7, 224));
    case 7:
      matrix->drawPixel(x - 2, y + 4, matrix->Color(0, 7, 224));
    case 6:
      matrix->drawPixel(x - 3, y + 4, matrix->Color(0, 7, 224));
    case 5:
      matrix->drawPixel(x - 3, y + 5, matrix->Color(0, 7, 224));
    case 4:
      matrix->drawPixel(x - 3, y + 6, matrix->Color(0, 7, 224));
    case 3:
      matrix->drawPixel(x - 3, y + 7, matrix->Color(0, 7, 224));
    case 2:
      matrix->drawPixel(x - 4, y + 7, matrix->Color(0, 7, 224));
    case 1:
      matrix->drawPixel(x - 5, y + 7, matrix->Color(0, 7, 224));
    case 0:
      break;
    }
  }
  matrix->show();
}

void Matrix::hardwareAnimatedSearch(int typ, int x, int y)
{
  for (int i = 0; i < 4; i++)
  {
    matrix->clear();
    matrix->setTextColor(0xFFFF);
    if (typ == 0)
    {
      matrix->setCursor(7, 6);
      matrix->print("WiFi");
    }
    else if (typ == 1)
    {
      matrix->setCursor(5, 6);
      matrix->print("Host");
    }
    switch (i)
    {
    case 3:
      matrix->drawPixel(x, y, matrix->Color(0, 7, 224));
      matrix->drawPixel(x + 1, y + 1, matrix->Color(0, 7, 224));
      matrix->drawPixel(x + 2, y + 2, matrix->Color(0, 7, 224));
      matrix->drawPixel(x + 3, y + 3, matrix->Color(0, 7, 224));
      matrix->drawPixel(x + 2, y + 4, matrix->Color(0, 7, 224));
      matrix->drawPixel(x + 1, y + 5, matrix->Color(0, 7, 224));
      matrix->drawPixel(x, y + 6, matrix->Color(0, 7, 224));
    case 2:
      matrix->drawPixel(x - 1, y + 2, matrix->Color(0, 7, 224));
      matrix->drawPixel(x, y + 3, matrix->Color(0, 7, 224));
      matrix->drawPixel(x - 1, y + 4, matrix->Color(0, 7, 224));
    case 1:
      matrix->drawPixel(x - 3, y + 3, matrix->Color(0, 7, 224));
    case 0:
      break;
    }
    matrix->show();
    delay(100);
  }
}

void Matrix::setType(int type){
    switch (type)
        {
        case 0:
            matrix = new FastLED_NeoMatrix(leds, 32, 8, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG);
            break;
        case 1:
            matrix = new FastLED_NeoMatrix(leds, 8, 8, 4, 1, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE);
            break;
        case 2:
            matrix = new FastLED_NeoMatrix(leds, 32, 8, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG);
            break;
        default:
            matrix = new FastLED_NeoMatrix(leds, 32, 8, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG);
            break;
    }
}

void Matrix::setTempCorrection(int tempCorrection, int PIN, int numberLEDs){
	/*
    switch (tempCorrection)
  {
  case 0:
    FastLED.addLeds<NEOPIXEL, PIN>(leds, 256).setCorrection(TypicalLEDStrip);
    break;
  case 1:
    FastLED.addLeds<NEOPIXEL, PIN>(leds, numberLEDs).setTemperature(Candle);
    break;
  case 2:
    FastLED.addLeds<NEOPIXEL, PIN>(leds, numberLEDs).setTemperature(Tungsten40W);
    break;
  case 3:
    FastLED.addLeds<NEOPIXEL, PIN>(leds, numberLEDs).setTemperature(Tungsten100W);
    break;
  case 4:
    FastLED.addLeds<NEOPIXEL, PIN>(leds, numberLEDs).setTemperature(Halogen);
    break;
  case 5:
    FastLED.addLeds<NEOPIXEL, PIN>(leds, numberLEDs).setTemperature(CarbonArc);
    break;
  case 6:
    FastLED.addLeds<NEOPIXEL, PIN>(leds, numberLEDs).setTemperature(HighNoonSun);
    break;
  case 7:
    FastLED.addLeds<NEOPIXEL, PIN>(leds, numberLEDs).setTemperature(DirectSunlight);
    break;
  case 8:
    FastLED.addLeds<NEOPIXEL, PIN>(leds, numberLEDs).setTemperature(OvercastSky);
    break;
  case 9:
    FastLED.addLeds<NEOPIXEL, PIN>(leds, numberLEDs).setTemperature(ClearBlueSky);
    break;
  case 10:
    FastLED.addLeds<NEOPIXEL, PIN>(leds, numberLEDs).setTemperature(WarmFluorescent);
    break;
  case 11:
    FastLED.addLeds<NEOPIXEL, PIN>(leds, numberLEDs).setTemperature(StandardFluorescent);
    break;
  case 12:
    FastLED.addLeds<NEOPIXEL, PIN>(leds, numberLEDs).setTemperature(CoolWhiteFluorescent);
    break;
  case 13:
    FastLED.addLeds<NEOPIXEL, PIN>(leds, numberLEDs).setTemperature(FullSpectrumFluorescent);
    break;
  case 14:
    FastLED.addLeds<NEOPIXEL, PIN>(leds, numberLEDs).setTemperature(GrowLightFluorescent);
    break;
  case 15:
    FastLED.addLeds<NEOPIXEL, PIN>(leds, numberLEDs).setTemperature(BlackLightFluorescent);
    break;
  case 16:
    FastLED.addLeds<NEOPIXEL, PIN>(leds, numberLEDs).setTemperature(MercuryVapor);
    break;
  case 17:
    FastLED.addLeds<NEOPIXEL, PIN>(leds, numberLEDs).setTemperature(SodiumVapor);
    break;
  case 18:
    FastLED.addLeds<NEOPIXEL, PIN>(leds, numberLEDs).setTemperature(MetalHalide);
    break;
  case 19:
    FastLED.addLeds<NEOPIXEL, PIN>(leds, numberLEDs).setTemperature(HighPressureSodium);
    break;
  case 20:
    FastLED.addLeds<NEOPIXEL, PIN>(leds, numberLEDs).setTemperature(UncorrectedTemperature);
    break;
  default:
    FastLED.addLeds<NEOPIXEL, PIN>(leds, numberLEDs).setCorrection(TypicalLEDStrip);
    break;
  }
  */
}