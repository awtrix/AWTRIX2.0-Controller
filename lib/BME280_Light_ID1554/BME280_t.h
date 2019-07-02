//based on:
// Adafruit Adafruit_BME280_Library
// https://github.com/adafruit/Adafruit_BME280_Library
//and
// Astuder BMP085-template-library-Energia
// https://github.com/astuder/BMP085-template-library-Energia
//plus code for altitude and relative pressure
//by r7

#ifndef BME280_T_h
#define BME280_T_h

#include <inttypes.h>
#include <Arduino.h>

//---------------------I2C ADDRESS---------------------------------------------------
#define BME280_ADDRESS                (0x76)
//---------------------REGISTERS-----------------------------------------------------
enum
{
  BME280_REGISTER_DIG_T1              = 0x88,
  BME280_REGISTER_DIG_T2              = 0x8A,
  BME280_REGISTER_DIG_T3              = 0x8C,

  BME280_REGISTER_DIG_P1              = 0x8E,
  BME280_REGISTER_DIG_P2              = 0x90,
  BME280_REGISTER_DIG_P3              = 0x92,
  BME280_REGISTER_DIG_P4              = 0x94,
  BME280_REGISTER_DIG_P5              = 0x96,
  BME280_REGISTER_DIG_P6              = 0x98,
  BME280_REGISTER_DIG_P7              = 0x9A,
  BME280_REGISTER_DIG_P8              = 0x9C,
  BME280_REGISTER_DIG_P9              = 0x9E,

  BME280_REGISTER_DIG_H1              = 0xA1,
  BME280_REGISTER_DIG_H2              = 0xE1,
  BME280_REGISTER_DIG_H3              = 0xE3,
  BME280_REGISTER_DIG_H4              = 0xE4,
  BME280_REGISTER_DIG_H5              = 0xE5,
  BME280_REGISTER_DIG_H6              = 0xE7,

  BME280_REGISTER_CHIPID              = 0xD0,
  BME280_REGISTER_VERSION             = 0xD1,
  BME280_REGISTER_SOFTRESET           = 0xE0,

  BME280_REGISTER_CAL26               = 0xE1,  // R calibration stored in 0xE1-0xF0

  BME280_REGISTER_CONTROLHUMID        = 0xF2,
  BME280_REGISTER_CONTROL             = 0xF4,
  BME280_REGISTER_CONFIG              = 0xF5,
  BME280_REGISTER_PRESSUREDATA        = 0xF7,
  BME280_REGISTER_TEMPDATA            = 0xFA,
  BME280_REGISTER_HUMIDDATA           = 0xFD,
};
//---------------------CALIBRATION DATA----------------------------------------------
typedef struct
{
  uint16_t dig_T1;
  int16_t  dig_T2;
  int16_t  dig_T3;

  uint16_t dig_P1;
  int16_t  dig_P2;
  int16_t  dig_P3;
  int16_t  dig_P4;
  int16_t  dig_P5;
  int16_t  dig_P6;
  int16_t  dig_P7;
  int16_t  dig_P8;
  int16_t  dig_P9;

  uint8_t  dig_H1;
  int16_t  dig_H2;
  uint8_t  dig_H3;
  int16_t  dig_H4;
  int16_t  dig_H5;
  int8_t   dig_H6;
} bme280_calib_data;
//---------------------TEMPERATURE UNITS----------------------------------------------
enum BME280_temp_t
{
  BME280_C,
  BME280_F
};
//---------------------TEMPLATE/STRUCTURE---------------------------------------------
template <BME280_temp_t tempunit = BME280_C,
          uint8_t _i2caddr = BME280_ADDRESS>
struct BME280 {

private:

  bme280_calib_data _bme280_calib;
  
  //---------------Writes an 8 bit value over I2C
  void write8(byte reg, byte value) {
    Wire.beginTransmission((uint8_t)_i2caddr);
    Wire.write((uint8_t)reg);
    Wire.write((uint8_t)value);
    Wire.endTransmission();
  }
  //---------------Reads an 8 bit value over I2C
  uint8_t read8(byte reg) {
    uint8_t value;

    Wire.beginTransmission((uint8_t)_i2caddr);
    Wire.write((uint8_t)reg);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)_i2caddr, (byte)1);
    value = Wire.read();
    Wire.endTransmission();

    return value;
  };
  //---------------Reads a 16 bit value over I2C
  uint16_t read16(byte reg) {
    uint16_t value;

    Wire.beginTransmission((uint8_t)_i2caddr);
    Wire.write((uint8_t)reg);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)_i2caddr, (byte)2);
    value = (Wire.read() << 8) | Wire.read();
    Wire.endTransmission();

    return value;
  };

  uint16_t read16_LE(byte reg) {
    uint16_t temp = read16(reg);

    return (temp >> 8) | (temp << 8);
  };
  //---------------Reads a signed 16 bit value over I2C
  int16_t readS16(byte reg) {
    return (int16_t)read16(reg);
  };

  int16_t readS16_LE(byte reg) {
    return (int16_t)read16_LE(reg);
  };
  
  //---------------Reads the factory-set coefficients
  void readCoefficients(void) {
    _bme280_calib.dig_T1 = read16_LE(BME280_REGISTER_DIG_T1);
    _bme280_calib.dig_T2 = readS16_LE(BME280_REGISTER_DIG_T2);
    _bme280_calib.dig_T3 = readS16_LE(BME280_REGISTER_DIG_T3);

    _bme280_calib.dig_P1 = read16_LE(BME280_REGISTER_DIG_P1);
    _bme280_calib.dig_P2 = readS16_LE(BME280_REGISTER_DIG_P2);
    _bme280_calib.dig_P3 = readS16_LE(BME280_REGISTER_DIG_P3);
    _bme280_calib.dig_P4 = readS16_LE(BME280_REGISTER_DIG_P4);
    _bme280_calib.dig_P5 = readS16_LE(BME280_REGISTER_DIG_P5);
    _bme280_calib.dig_P6 = readS16_LE(BME280_REGISTER_DIG_P6);
    _bme280_calib.dig_P7 = readS16_LE(BME280_REGISTER_DIG_P7);
    _bme280_calib.dig_P8 = readS16_LE(BME280_REGISTER_DIG_P8);
    _bme280_calib.dig_P9 = readS16_LE(BME280_REGISTER_DIG_P9);

    _bme280_calib.dig_H1 = read8(BME280_REGISTER_DIG_H1);
    _bme280_calib.dig_H2 = readS16_LE(BME280_REGISTER_DIG_H2);
    _bme280_calib.dig_H3 = read8(BME280_REGISTER_DIG_H3);
    _bme280_calib.dig_H4 = (read8(BME280_REGISTER_DIG_H4) << 4) | (read8(BME280_REGISTER_DIG_H4+1) & 0xF);
    _bme280_calib.dig_H5 = (read8(BME280_REGISTER_DIG_H5+1) << 4) | (read8(BME280_REGISTER_DIG_H5) >> 4);
    _bme280_calib.dig_H6 = (int8_t)read8(BME280_REGISTER_DIG_H6);
  };

public:
  float temperature = 0;
  float pressure = 0;
  float humidity = 0;
  
  bool begin(int sdaPin, int sclPin) {
    Wire.begin(sdaPin,sclPin);

    if (read8(BME280_REGISTER_CHIPID) != 0x58)  //vorher 0x60
      return false;

    readCoefficients();
    write8(BME280_REGISTER_CONTROLHUMID, 0x03); // Set before CONTROL (DS 5.4.3)
    write8(BME280_REGISTER_CONTROL, 0x3F);
    return true;  
  };
  
  void refresh() {
    //------------------temperature
    //int32_t var1, var2;

    int32_t adc_T = read16(BME280_REGISTER_TEMPDATA);
    adc_T <<= 8;
    adc_T |= read8(BME280_REGISTER_TEMPDATA+2);
    adc_T >>= 4;

    int32_t var1t  = ((((adc_T>>3) - ((int32_t)_bme280_calib.dig_T1 <<1))) *
       ((int32_t)_bme280_calib.dig_T2)) >> 11;

    int32_t var2t  = (((((adc_T>>4) - ((int32_t)_bme280_calib.dig_T1)) *
         ((adc_T>>4) - ((int32_t)_bme280_calib.dig_T1))) >> 12) *
       ((int32_t)_bme280_calib.dig_T3)) >> 14;

    int32_t t_fine = var1t + var2t;

    float T  = (t_fine * 5 + 128) >> 8;
    temperature = T/100;    

    //------------------pressure
    int64_t var1, var2, p;

    int32_t adc_P = read16(BME280_REGISTER_PRESSUREDATA);
    adc_P <<= 8;
    adc_P |= read8(BME280_REGISTER_PRESSUREDATA+2);
    adc_P >>= 4;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)_bme280_calib.dig_P6;
    var2 = var2 + ((var1*(int64_t)_bme280_calib.dig_P5)<<17);
    var2 = var2 + (((int64_t)_bme280_calib.dig_P4)<<35);
    var1 = ((var1 * var1 * (int64_t)_bme280_calib.dig_P3)>>8) +
      ((var1 * (int64_t)_bme280_calib.dig_P2)<<12);
    var1 = (((((int64_t)1)<<47)+var1))*((int64_t)_bme280_calib.dig_P1)>>33;

    if (var1 == 0) {
      //return 0;  // avoid exception caused by division by zero
      return;  // avoid exception caused by division by zero
    }
    p = 1048576 - adc_P;
    p = (((p<<31) - var2)*3125) / var1;
    var1 = (((int64_t)_bme280_calib.dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((int64_t)_bme280_calib.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)_bme280_calib.dig_P7)<<4);
    pressure = (float)p/256;
    
    //------------------humidity
    int32_t adc_H = read16(BME280_REGISTER_HUMIDDATA);

    int32_t v_x1_u32r;

    v_x1_u32r = (t_fine - ((int32_t)76800));

    v_x1_u32r = (((((adc_H << 14) - (((int32_t)_bme280_calib.dig_H4) << 20) -
        (((int32_t)_bme280_calib.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
           (((((((v_x1_u32r * ((int32_t)_bme280_calib.dig_H6)) >> 10) *
          (((v_x1_u32r * ((int32_t)_bme280_calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
        ((int32_t)2097152)) * ((int32_t)_bme280_calib.dig_H2) + 8192) >> 14));

    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
             ((int32_t)_bme280_calib.dig_H1)) >> 4));

    v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
    v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
    float h = (v_x1_u32r>>12);
    humidity = h / 1024.0;

  };

  //float pressureToAltitude(float seaLevel, float atmospheric) {
  float pressureToAltitude(float seaLevel = 101325.00) { 
    ////float atmospheric = pressure / 100.0F;    //hPa
    //float atmospheric = pressure;               //Pa
    //return 44330.0 * (1.0 - pow(atmospheric / seaLevel, 0.1903));
    return 44330.0 * (1.0 - pow(pressure / seaLevel, 0.1903));
  };
  
  //float seaLevelForAltitude(float altitude = 150.50, float atmospheric) {
  float seaLevelForAltitude(float altitude = 150.50) {
    ////float atmospheric = pressure / 100.0F;    //hPa
    //float atmospheric = pressure;               //Pa    
    //return pow((altitude/44330.0) + 1.0, 5.255F) * atmospheric;
    return pow((altitude / 44330.0) + 1.0, 5.255F) * pressure;
  };
 
};
#endif
