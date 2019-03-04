/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#include "eq_light_sensor.h"
#include "eq_display.h"
#include "eq_dpin.h"

void EqLightSensor::collect_() {
  if (EqConfig::lightSensorIsAnalog) {
    uint16_t __l = analogRead(EqConfig::lightSensorPin);
    collector_.add(EqConfig::lightSensorInvert ? 1023 - __l : __l);
  } else { // digital light sensor
    bool __s = EqConfig::lightSensorInvert
                   ? EqDPin<EqConfig::lightSensorPin>::isInputLow()
                   : EqDPin<EqConfig::lightSensorPin>::isInputHigh();
    collector_.add(__s ? 1023 : 0);
  }
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
  const char *__s = EqConfig::alertAsString(EqAlertType::LightSensor);
#if defined(EQ_DEBUG)
  Serial.print(__s);
#endif
  eqDisplay().showMessage(__s);
  EqDPin<EqConfig::lightSensorPin>::setInput();
  for (uint8_t __i = 0; __i < EqConfig::lightSensorCollectorSize; __i++) {
    delay(200);
    collect_();
  }
  if (collector_.deviation() > EqConfig::sensorMaxDeviation) {
    EqConfig::setAlert(EqAlertType::LightSensor);
    return false; // to much sensor deviation
  }
  return true;
}

// instance of EqLightSensor
EqLightSensor EqLightSensor::instance_{};
