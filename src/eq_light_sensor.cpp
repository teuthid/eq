
#include "eq_light_sensor.h"

#include <FastGPIO.h>

void EqLightSensor::collect_() {
  if (EqConfig::lightSensorIsAnalog)
    collector_.add(analogRead(EqConfig::lightSensorPin));
  else
    collector_.add(FastGPIO::Pin<EqConfig::lightSensorPin>::isInputHigh() ? 1023
                                                                          : 0);
}

void EqLightSensor::determineState_() {
  intensity_ = map(collector_.averageAsLong(), 0, 1023, 0, 100);
  if (intensity_ >= EqConfig::lightSensorThreshold())
    state_ = (state_ == State::Low) ? State::Rising : State::High;
  else
    state_ = State::Low;
  if (state_ == State::Rising)
    EqConfig::setBacklight();
}

void EqLightSensor::read() {
  collect_();
  determineState_();
}

bool EqLightSensor::init() {
#ifdef EQ_DEBUG
  Serial.print(F("[Light Sensor] "));
#endif
  FastGPIO::Pin<EqConfig::lightSensorPin>::setInput();
  for (uint8_t __i = 0; __i < EqConfig::lightSensorCollectorSize; __i++) {
    delay(200);
    collect_();
  }
  if (collector_.deviation() > EqConfig::sensorMaxDeviation) {
    EqConfig::setAlert(EqAlertType::LightSensor);
    return false;
  }
  return true;
}
