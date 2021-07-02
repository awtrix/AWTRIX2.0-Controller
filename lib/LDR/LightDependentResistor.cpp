/*
 * \brief et light intensity from Light dependent Resistor (implementation)
 *
 * \author Quentin Comte-Gaz <quentin@comte-gaz.com>
 * \date 4 July 2016
 * \license MIT License (contact me if too restrictive)
 * \copyright Copyright (c) 2016 Quentin Comte-Gaz
 * \version 1.0
 */

#include "LightDependentResistor.h"

LightDependentResistor::LightDependentResistor(int pin, unsigned long other_resistor, ePhotoCellKind kind) :
  _photocell_on_ground (true),
  _pin (pin),
  _other_resistor (other_resistor)
{
  switch (kind) {
    case GL5516:
      _mult_value = 29634400;
      _pow_value = 1.6689;
    break;
    case GL5537_1:
      _mult_value = 32435800;
      _pow_value = 1.4899;
    break;
    case GL5537_2:
      _mult_value = 2801820;
      _pow_value = 1.1772;
    break;
    case GL5539:
      _mult_value = 208510000;
      _pow_value = 1.4850;
    break;
    case GL5549:
      _mult_value = 44682100;
      _pow_value = 1.2750;
    break;
    case GL5528:
    default:
      _mult_value = 32017200;
      _pow_value = 1.5832;
    }
}

LightDependentResistor::LightDependentResistor(int pin, unsigned long other_resistor, float mult_value, float pow_value) :
  _photocell_on_ground (true),
  _pin (pin),
  _other_resistor (other_resistor),
  _mult_value (mult_value),
  _pow_value (pow_value)
{
}

void LightDependentResistor::updatePhotocellParameters(float mult_value, float pow_value)
{
  _mult_value = mult_value;
  _pow_value = pow_value;
}

void LightDependentResistor::updateResistor(unsigned long other_resistor)
{
  _other_resistor = other_resistor;
}

float LightDependentResistor::luxToFootCandles(float intensity_in_lux)
{
  return 10.764*intensity_in_lux;
}

float LightDependentResistor::footCandlesToLux(float intensity_in_footcandles)
{
  return intensity_in_footcandles/10.764;
}

void LightDependentResistor::setPhotocellPositionOnGround(bool on_ground)
{
  _photocell_on_ground = on_ground;
}

float LightDependentResistor::getCurrentLux() const
{
  int photocell_value = analogRead(_pin);

  unsigned long photocell_resistor;

  float ratio = ((float)1024/(float)photocell_value) - 1;
  if (_photocell_on_ground) {
    photocell_resistor = _other_resistor / ratio;
  } else {
    photocell_resistor = _other_resistor * ratio;
  }

  return _mult_value / (float)pow(photocell_resistor, _pow_value);
}

float LightDependentResistor::getCurrentFootCandles() const
{
  return luxToFootCandles(getCurrentLux());
}
