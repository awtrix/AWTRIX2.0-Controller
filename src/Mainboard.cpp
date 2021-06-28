#include <SPI.h>
#include "Mainboard.h"
#include <Arduino.h>
#include <HardwareSerial.h>
//#include "Matrix.h"
//#include <SoftwareSerial.h>

//Matrix myMatrix;

void Mainboard::setup(){
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
    digitalWrite(STATUS_LED_PIN, LEDstate);
    #endif
}
/// Inizialized the matrix with the parameter
/// \param type: type of the matrix: \n
/// \param type0: NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG
/// \param type1: NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE
/// \param type2: NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG
/// \param type3: NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG

/// \param tempCorrection: temperatur correction (the list is too long to include here)
void Mainboard::setupMatrix(int type, int tempCorrection){
    //myMatrix.init(type,tempCorrection, MATRIX_PIN);
}

void Mainboard::setTextToMatrix(bool *clear, byte *red, byte *green, byte *blue, int *xPos, int *yPos, String *text){
    //myMatrix.setTextToMatrix(clear, red, green, blue, xPos, yPos, text);
}