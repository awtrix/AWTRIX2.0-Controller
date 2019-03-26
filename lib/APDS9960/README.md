SparkFun APDS9960 RGB and Gesture Sensor Arduino Library -- Modified for esp8266 
=========================================================

Made a few changes to get this to work with esp8266.

The main two are:
* Removing wire.begin() from SparkFun_APDS9960.cpp and moving it into the examples so that the pins it uses can be specified in your sketch

* Changed the LED_BOOST_300 to LED_BOOST_100 in SparkFun_APDS9960.cpp as I couldn't get the gesture sensor to work without changing this

See Sparkfun's original library here https://github.com/sparkfun/SparkFun_APDS-9960_Sensor_Arduino_Library for usage.
