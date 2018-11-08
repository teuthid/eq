
#ifndef __EQ_TEMP_SENSOR_H__
#define __EQ_TEMP_SENSOR_H__

#include <DallasTemperature.h>
#include <OneWire.h>

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
};

bool __EqTempSensor::init() {
  sensor_.begin();
  if (!sensor_.getAddress(address_, 0))
    return false;
  sensor_.setResolution(address_, 12);
  return true;
}

float __EqTempSensor::read() {
  sensor_.requestTemperatures();
  while (!sensor_.isConversionComplete())
    ;
  return sensor_.getTempC(address_);
}

// specializations for DS18B20
template <> struct __EqHtSensorObject<EQ_DS18B20> {
  typedef __EqTempSensor Type;
};

template <>
EqHtSensor<EQ_DS18B20, false>::EqHtSensor(const uint8_t &sensorPin)
    : sensor_(sensorPin) {}

template <> bool EqHtSensor<EQ_DS18B20, false>::initHtSensor_() {
  return sensor_.init();
}
template <>
void EqHtSensor<EQ_DS18B20, false>::readHTSensor_(float &humidity,
                                                  float &temperature) {
  humidity = 0;
  temperature = sensor_.read();
}

typedef EqHtSensor<EQ_DS18B20, false> EqTempSensor;

#endif // __EQ_TEMP_SENSOR_H__
