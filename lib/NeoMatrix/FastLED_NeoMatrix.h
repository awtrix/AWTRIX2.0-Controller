/*--------------------------------------------------------------------
  Arduino library based on Adafruit_Neomatrix but modified to work with FastLED
  by Marc MERLIN <marc_soft@merlins.org>

  Original notice and license from Adafruit_Neomatrix:
  NeoMatrix is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  NeoMatrix is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with NeoMatrix.  If not, see
  <http://www.gnu.org/licenses/>.
  --------------------------------------------------------------------*/

#ifndef _FASTLED_NEOMATRIX_H_
#define _FASTLED_NEOMATRIX_H_

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif
#include <Adafruit_GFX.h>
#if defined(ESP8266)
// If you get matrix flickering, modify platforms/esp/8266/clockless_esp8266.h 
// and platforms/esp/8266/clockless_block_esp8266.h to change WAIT_TIME to 20
//#pragma message "If you get matrix corruption, turn off FASTLED_ALLOW_INTERRUPTS"
//#pragma message "in this library, or modify WAIT_TIME in platforms/esp/8266/clockless_esp8266.h"
//#pragma message "(raise it from 5 to 20 or up to 50 if needed)"
// Or if you don't need interrupts, you can disable them here
//#define FASTLED_ALLOW_INTERRUPTS 0
#endif
#include <FastLED.h>

// Matrix layout information is passed in the 'matrixType' parameter for
// each constructor (the parameter immediately following is the LED type
// from NeoPixel.h).

// These define the layout for a single 'unified' matrix (e.g. one made
// from NeoPixel strips, or a single NeoPixel shield), or for the pixels
// within each matrix of a tiled display (e.g. multiple NeoPixel shields).

#define NEO_MATRIX_TOP         0x00 // Pixel 0 is at top of matrix
#define NEO_MATRIX_BOTTOM      0x01 // Pixel 0 is at bottom of matrix
#define NEO_MATRIX_LEFT        0x00 // Pixel 0 is at left of matrix
#define NEO_MATRIX_RIGHT       0x02 // Pixel 0 is at right of matrix
#define NEO_MATRIX_CORNER      0x03 // Bitmask for pixel 0 matrix corner
#define NEO_MATRIX_ROWS        0x00 // Matrix is row major (horizontal)
#define NEO_MATRIX_COLUMNS     0x04 // Matrix is column major (vertical)
#define NEO_MATRIX_AXIS        0x04 // Bitmask for row/column layout
#define NEO_MATRIX_PROGRESSIVE 0x00 // Same pixel order across each line
#define NEO_MATRIX_ZIGZAG      0x08 // Pixel order reverses between lines
#define NEO_MATRIX_SEQUENCE    0x08 // Bitmask for pixel line order

// These apply only to tiled displays (multiple matrices):

#define NEO_TILE_TOP           0x00 // First tile is at top of matrix
#define NEO_TILE_BOTTOM        0x10 // First tile is at bottom of matrix
#define NEO_TILE_LEFT          0x00 // First tile is at left of matrix
#define NEO_TILE_RIGHT         0x20 // First tile is at right of matrix
#define NEO_TILE_CORNER        0x30 // Bitmask for first tile corner
#define NEO_TILE_ROWS          0x00 // Tiles ordered in rows
#define NEO_TILE_COLUMNS       0x40 // Tiles ordered in columns
#define NEO_TILE_AXIS          0x40 // Bitmask for tile H/V orientation
#define NEO_TILE_PROGRESSIVE   0x00 // Same tile order across each line
#define NEO_TILE_ZIGZAG        0x80 // Tile order reverses between lines
#define NEO_TILE_SEQUENCE      0x80 // Bitmask for tile line order

/* 
 * Ideally FastLED_NeoMatrix would multiple inherit from CFastLED too
 * I tried this, but on that path laid madness, apparent compiler bugs
 * and pain due to the unfortunate use of templates in FastLED, preventing
 * passing initalization arguments in the object since they need to be
 * hardcoded at compile time as template values :( -- merlin
 */
class FastLED_NeoMatrix : public Adafruit_GFX {

 public:
  // pre-computed gamma table
  uint8_t gamma[256];

  // Constructor for single matrix:
  FastLED_NeoMatrix(CRGB *, uint8_t w, uint8_t h, 
    uint8_t matrixType = NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS);

  // Constructor for tiled matrices:
  FastLED_NeoMatrix(CRGB *, uint8_t matrixW, uint8_t matrixH, 
    uint8_t tX, uint8_t tY,
    uint8_t matrixType = NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS +
                         NEO_TILE_TOP + NEO_TILE_LEFT + NEO_TILE_ROWS);


  int XY(int16_t x, int16_t y); // compat with FastLED code, returns 1D offset
  void
    drawPixel(int16_t x, int16_t y, uint16_t color),
    fillScreen(uint16_t color),
    setPassThruColor(uint32_t c),
    setPassThruColor(void),
    setRemapFunction(uint16_t (*fn)(uint16_t, uint16_t)),
    precal_gamma(float);

  static uint16_t
    Color(uint8_t r, uint8_t g, uint8_t b);

  void clear() { FastLED.clear(); };
  void setBrightness(int b) { FastLED.setBrightness(b); };

  void show() {
#ifdef ESP8266
// Disable watchdog interrupt so that it does not trigger in the middle of
// updates. and break timing of pixels, causing random corruption on interval
// https://github.com/esp8266/Arduino/issues/34
    ESP.wdtDisable();
#endif
    FastLED.show();
#ifdef ESP8266
    ESP.wdtEnable(1000);
#endif
  };

  void begin();

 private:

  CRGB *_leds;
  const uint8_t
    type;
  const uint8_t
    matrixWidth, matrixHeight, tilesX, tilesY;
  uint16_t
    numpix,
    (*remapFn)(uint16_t x, uint16_t y);

  uint32_t passThruColor;
  boolean  passThruFlag = false;
};

#endif // _FASTLED_NEOMATRIX_H_
// vim:sts=2:sw=2
