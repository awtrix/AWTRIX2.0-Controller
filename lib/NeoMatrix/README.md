FastLED NeoMatrix
=================

Adafruit_GFX-compatible library for NeoPixel matrices and grids. Controls single and tiled NeoPixel displays.
This library requires FastLED and Adafruit_GFX libraries.

Please see https://learn.adafruit.com/adafruit-neopixel-uberguide/neomatrix-library for how to use this library.

Example code: https://github.com/marcmerlin/FastLED_NeoMatrix/blob/master/examples/MatrixGFXDemo/MatrixGFXDemo.ino

More complex example code: https://github.com/marcmerlin/NeoMatrix-FastLED-IR

This code was taken from Adafruit_NeoMatrix and adapted to work with the more powerful FastLED library.
The reasons for using FastLED instead of Adafruit::Neopixel as a backend, include:
* FastLED supports more microcontrollers
* Better support for ESP32 than Adafruit::Neopixel
* Support for parallel output on some chips for faster refresh rate: https://github.com/FastLED/FastLED/wiki/Parallel-Output
* Support for interrupts to allow things like Infrared to work while updating strips
* Support for many more led/pixel hardware: https://github.com/FastLED/FastLED/wiki/Chipset-reference

Video demo: https://www.youtube.com/watch?v=tU_wkrrv_4A

![164_20170424_adafruit_gfx_on_neomatrix_32x32](https://user-images.githubusercontent.com/1369412/38774532-5d6b0f2e-4020-11e8-86ef-afdffbeb1e1d.jpg)
![171_20170424_adafruit_gfx_on_neomatrix_32x32](https://user-images.githubusercontent.com/1369412/38774533-5d83d6bc-4020-11e8-95bb-417368143d70.jpg)
