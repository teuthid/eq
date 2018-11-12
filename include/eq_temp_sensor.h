
#ifndef __EQ_TEMP_SENSOR_H__
#define __EQ_TEMP_SENSOR_H__

#include <DallasTemperature.h>
#include <OneWire.h>
#include <float.h>

#include "eq_ht_sensor.h"

class __EqTempSensor {
public:
  __EqTempSensor(const uint8_t &sensorPin = EqConfig::htSensorPin)
      : onewire_(sensorPin), sensor_(&onewire_) {}

  bool init();
  float read();

private:
  OneWire onewire_;
  DallasTemperature sensor_;
  DeviceAddress address_;
  bool isCorrect_(float temp) const;
};

bool __EqTempSensor::isCorrect_(float temp) const {
  return (fabs(temp + 127.0) >= FLT_EPSILON);
}

bool __EqTempSensor::init() {
  sensor_.begin();
  if (!sensor_.getAddress(address_, 0))
    return false;
  sensor_.setResolution(address_, 12);
  return true;
}

float __EqTempSensor::read() {
  float __t;
  do {
    sensor_.requestTemperatures();
    __t = sensor_.getTempC(address_);
  } while (!isCorrect_(__t)); // FIXME: waiting for watchdog
  return __t;
}

// specializations for DS18B20
template <> struct __EqHtSensorObject<EQ_DS18B20> {
  typedef __EqTempSensor Type;
};

template <>
EqHtSensor<EQ_DS18B20, false>::EqHtSensor(const uint8_t &sensorPin)
    : sensor_(sensorPin) {}
template <> uint16_t EqHtSensor<EQ_DS18B20, false>::samplingPeriod_() const {
  return 1000;
}
template <> bool EqHtSensor<EQ_DS18B20, false>::initHtSensor_() {
  return sensor_.init();
}
template <>
void EqHtSensor<EQ_DS18B20, false>::readHTSensor_(float &humidity,
                                                  float &temperature) {
  humidity = 0;
  noInterrupts();
  temperature = sensor_.read();
  interrupts();
}

typedef EqHtSensor<EQ_DS18B20, false> EqTempSensor;

#endif // __EQ_TEMP_SENSOR_H__
