
#include "eq_ht_sensor.h"

// --- AM2320 -----------------------------------------------------------------
#if (EQ_HT_SENSOR_TYPE == EQ_AM2320)
#include <AM232X.h>

namespace {
AM232X __htSensor;
}

template <> bool EqHtSensor<EQ_AM2320, false>::initHtSensor_() {
  return (__htSensor.read() == AM232X_OK);
}

template <>
void EqHtSensor<EQ_AM2320, false>::readHTSensor_(fixed_t &humidity,
                                                 fixed_t &temperature) {
  if (__htSensor.read() == AM232X_OK) {
    humidity = __htSensor.humidity;
    temperature = __htSensor.temperature;
  }
}
#endif // (EQ_HT_SENSOR_TYPE == EQ_AM2320)
