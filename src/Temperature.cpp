#include "Temperature.h"

int Temperature::init(){
    isInit = true;
    Wire.begin(I2C_SDA, I2C_SCL);
	if (BMESensor.begin())
	{
		currentTempSensor = TEMPSENSOR_BME280;
		//hardwareAnimatedCheck(MsgType_Temp, 29, 2);
        updateValues();
        return TEMPSENSOR_BME280;
	}
	else if (htu.begin())
	{
		currentTempSensor = TEMPSENSOR_HTU21D;
		//hardwareAnimatedCheck(MsgType_Temp, 29, 2);
        updateValues();
        return TEMPSENSOR_HTU21D;
	}
	else if (BMPSensor.begin(BMP280_ADDRESS_ALT) || BMPSensor.begin(BMP280_ADDRESS))
	{
		/* Default settings from datasheet. */
		BMPSensor.setSampling(Adafruit_BMP280::MODE_NORMAL,		/* Operating Mode. */
							  Adafruit_BMP280::SAMPLING_X2,		/* Temp. oversampling */
							  Adafruit_BMP280::SAMPLING_X16,	/* Pressure oversampling */
							  Adafruit_BMP280::FILTER_X16,		/* Filtering. */
							  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
		currentTempSensor = TEMPSENSOR_BMP280;
        updateValues();
        return TEMPSENSOR_BMP280;
		//hardwareAnimatedCheck(MsgType_Temp, 29, 2);
	} else {
        currentTempSensor = TEMPSENSOR_NONE;
        updateValues();
    }
    return 0;
}

void Temperature::updateValues(){
    switch(currentTempSensor){
        case 0:
            temp = 0.0;
            presure = 0.0;
            humidity = 0.0;
            break;
        case 1:
            BMESensor.refresh();
            temp = BMESensor.temperature;
            humidity = BMESensor.humidity;
            presure = BMESensor.pressure;
            break;
        case 2:
            temp = htu.readTemperature();
            humidity = htu.readHumidity();
            presure = 0.0;
            break;
        case 3:
            sensors_event_t temp_event, pressure_event;
            BMPSensor.getTemperatureSensor()->getEvent(&temp_event);
            BMPSensor.getPressureSensor()->getEvent(&pressure_event);
            temp = temp_event.temperature;
            presure = pressure_event.pressure;
            humidity = 0.0;
            break;
    }
}

float Temperature::getTemperature(){
    return temp;
}

float Temperature::getHumidity(){
    return humidity;
}

float Temperature::getPresure(){
    return presure;
}

void Temperature::printReadings(){
    if(isInit){
        if(currentTempSensor>0){
            this->updateValues();
            Serial.println("------------------");
            Serial.println("Temperatur Test");
            Serial.print("Temeratursensor: ");
            Serial.println(currentTempSensor);
            
            Serial.print("Temp: ");
            Serial.print(temp);
            Serial.println(" °C");

            Serial.print("Hum: ");
            Serial.print(humidity);
            Serial.println(" %");

            Serial.print("Presure: ");
            Serial.print(presure);
            Serial.println(" hPa");
            Serial.println("------------------");
        }else {
            Serial.println("------------------");
            Serial.println("Temperatur Test");
            Serial.println("No temperature sensor connected...");
            Serial.println("------------------");
        }
        
    }else{
        Serial.println("------------------");
        Serial.println("Temperatur Test");
        Serial.println("Please use init() before use...");
        Serial.println("------------------");
    }
    
}
