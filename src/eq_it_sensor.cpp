
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
  fixed_t read();

private:
  OneWire onewire_;
  DallasTemperature sensor_;
  DeviceAddress address_;
  bool isCorrect_(const fixed_t &temp) const;
};

bool EqDallas::isCorrect_(const fixed_t &temp) const {
  return fixed_to_int(temp) != 127;
}

bool EqDallas::init() {
  sensor_.begin();
  if (!sensor_.getAddress(address_, 0))
    return false;
  sensor_.setResolution(address_, 12);
  return true;
}

fixed_t EqDallas::read() {
  fixed_t __t;
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

template <> bool EqHtSensor<EQ_DS18B20, true>::initHtSensor_() {
  return __itSensor.init();
}

template <>
void EqHtSensor<EQ_DS18B20, true>::readHTSensor_(fixed_t &humidity,
                                                 fixed_t &temperature) {
  temperature = __itSensor.read();
}

// specializations for DS18B20 (external sensor)
#if (EQ_HT_SENSOR_TYPE == EQ_DS18B20)
EqDallas __htSensor(EqConfig::htSensorPin);
#endif

// TO DO
