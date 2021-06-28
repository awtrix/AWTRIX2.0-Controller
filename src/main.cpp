// AWTRIX Controller
// Copyright (C) 2020
// by Blueforcer & Mazze2000

#include <Arduino.h>
#include "MenueControl/MenueControl.h"
#include "ESP.cpp"
#include "Mainboard.h"
#include "Matrix.h"
#include "Storage.h"
#include "Temperature.h"

ESPControl espControl;
Mainboard mainboard;
Matrix matrix;
Storage storage;
Temperature temperature;

void setup(){
    Serial.begin(115200);

    mainboard.init();
    matrix.init(0,0);
    storage.init();
    temperature.init();


    temperature.printReadings();
    mainboard.controlLED(!mainboard.ledState);

    matrix.setTextToMatrix(true,(byte)0,(byte)0,(byte)100,9,6,"Hallo");
}

void loop(){

}