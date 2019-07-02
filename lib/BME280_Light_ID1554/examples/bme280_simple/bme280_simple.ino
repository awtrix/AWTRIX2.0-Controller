//based on:
// Adafruit Adafruit_BME280_Library
// https://github.com/adafruit/Adafruit_BME280_Library
//and
// Astuder BMP085-template-library-Energia
// https://github.com/astuder/BMP085-template-library-Energia
//plus code for altitude and relative pressure
//by r7

#include <Wire.h>                                                       // required by BME280 library
#include <BME280_t.h>                                                   // import BME280 template library

#define ASCII_ESC 27

#define MYALTITUDE  150.50

char bufout[10];

BME280<> BMESensor;                                                     // instantiate sensor

void setup()
{
  Serial.begin(115200);                                                 // initialize serial
  Wire.begin(0,2);                                                      // initialize I2C that connects to sensor
  BMESensor.begin();                                                    // initalize bme280 sensor
}

void loop() {
  BMESensor.refresh();                                                  // read current sensor data
  sprintf(bufout,"%c[1;0H",ASCII_ESC);
  Serial.print(bufout);

  Serial.print("Temperature: ");
  Serial.print(BMESensor.temperature);                                  // display temperature in Celsius
  Serial.println("C");

  Serial.print("Humidity:    ");
  Serial.print(BMESensor.humidity);                                     // display humidity in %   
  Serial.println("%");

  Serial.print("Pressure:    ");
  Serial.print(BMESensor.pressure  / 100.0F);                           // display pressure in hPa
  Serial.println("hPa");

  float relativepressure = BMESensor.seaLevelForAltitude(MYALTITUDE);
  Serial.print("RelPress:    ");
  Serial.print(relativepressure  / 100.0F);                             // display relative pressure in hPa for given altitude
  Serial.println("hPa");   

  Serial.print("Altitude:    ");
  Serial.print(BMESensor.pressureToAltitude(relativepressure));         // display altitude in m for given pressure
  Serial.println("m");

  delay(1000);                                                          // wait a while before next loop
}
