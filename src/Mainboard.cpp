#include "Mainboard.h"

//#include "Matrix.h"
//#include <SoftwareSerial.h>

//Matrix myMatrix;

void Mainboard::init(){
    pinMode(STATUS_LED_PIN, OUTPUT);

    pinMode(BUTTON_LEFT_PIN, INPUT);
    pinMode(BUTTON_LEFT_PIN, INPUT_PULLUP);

    pinMode(BUTTON_MIDD_PIN, INPUT);
    pinMode(BUTTON_MIDD_PIN, INPUT_PULLUP);

    pinMode(BUTTON_RIGHT_PIN, INPUT);
    pinMode(BUTTON_RIGHT_PIN, INPUT_PULLUP);

    Serial2.begin(9600, SERIAL_8N1, DFP_TX_PIN, DFP_RX_PIN);
}

void Mainboard::controlLED(bool LEDstate){
    #ifdef ESP32
    ledState = LEDstate;
    digitalWrite(STATUS_LED_PIN, LEDstate);
    #endif
}