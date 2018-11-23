
#include <DallasTemperature.h>
#include <OneWire.h>
#include <float.h>

#include "eq_ht_sensor.h"

class EqDallas {
public:
  EqDallas(const uint8_t &sensorPin)
      : onewire_(sensorPin), sensor_(&onewire_) {}

  bool init();
  float read();

private:
  OneWire onewire_;
  DallasTemperature sensor_;
  DeviceAddress address_;
  bool isCorrect_(float temp) const;
};

bool EqDallas::isCorrect_(float temp) const {
  return (fabs(temp + 127.0) >= FLT_EPSILON);
}

bool EqDallas::init() {
  sensor_.begin();
  if (!sensor_.getAddress(address_, 0))
    return false;
  sensor_.setResolution(address_, 12);
  return true;
}

float EqDallas::read() {
  float __t;
  do {
    noInterrupts();
    sensor_.requestTemperatures();
    __t = sensor_.getTempC(address_);
    interrupts();
  } while (!isCorrect_(__t)); // FIXME: waiting for watchdog
  return __t;
}

EqDallas __itSensor(EqConfig::itSensorPin);

// specializations for DS18B20
/*
template <>
EqHtSensor<EQ_DS18B20, false>::EqHtSensor(const uint8_t &sensorPin) {
  sensor_ = new __EqTempSensor(sensorPin);
}

template <> uint16_t EqHtSensor<EQ_DS18B20, false>::samplingPeriod_() const {
  return 1000;
}

template <> bool EqHtSensor<EQ_DS18B20, false>::initHtSensor_() {
  return static_cast<__EqTempSensor *>(sensor_)->init();
}

template <>
void EqHtSensor<EQ_DS18B20, false>::readHTSensor_(float &humidity,
                                                  float &temperature) {
  temperature = static_cast<__EqTempSensor *>(sensor_)->read();
}
*/

#if (EQ_HT_SENSOR_TYPE == EQ_DS18B20)
EqDallas __htSensor(EqConfig::htSensorPin);
#endif

// TO DO
