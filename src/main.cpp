#include <Arduino.h>
#include "MenueControl/MenueControl.h"
#include "ESP.cpp"
#include "Mainboard.h"
#include "Matrix.h"


ESPControl espControl;
Mainboard mainboard;
Matrix matrix;

void setup(){
    Serial.begin(9600);
    mainboard.setup();
    mainboard.controlLED(true);
    matrix.init(0,0,15);
    matrix.setTextToMatrix(true,(byte)5,(byte)5,(byte)5,0,2,"Hallo");
}

void loop(){

}