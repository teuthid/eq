
#include <DallasTemperature.h>
#include <OneWire.h>
#include <float.h>

#include "eq_ht_sensor.h"

namespace {

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

} // namespace

// specializations for DS18B20 (internal sensor)
EqHtSensor<EQ_DS18B20, true> eqItSensor;

template <> uint16_t EqHtSensor<EQ_DS18B20, true>::samplingPeriod_() const {
  return 1000;
}

template <> bool EqHtSensor<EQ_DS18B20, true>::initHtSensor_() {
  return __itSensor.init();
}

template <>
void EqHtSensor<EQ_DS18B20, true>::readHTSensor_(float &humidity,
                                                 float &temperature) {
  temperature = __itSensor.read();
}

// specializations for DS18B20 (external sensor)
#if (EQ_HT_SENSOR_TYPE == EQ_DS18B20)
EqDallas __htSensor(EqConfig::htSensorPin);
#endif

// TO DO
