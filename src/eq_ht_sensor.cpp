
#include "eq_ht_sensor.h"

/*
  sensor specializations:
*/

// --- AM2320 -----------------------------------------------------------------
#if (EQ_HT_SENSOR_TYPE == EQ_AM2320)
#include <AM232X.h>

namespace {
AM232X __htSensor;
}

EqHtSensor<EQ_AM2320> eqHtSensor;

template <> uint16_t EqHtSensor<EQ_AM2320, true>::samplingPeriod_() const {
  return 2000;
}

template <> bool EqHtSensor<EQ_AM2320, true>::initHtSensor_() {
  return (__htSensor.read() == AM232X_OK);
}

template <>
void EqHtSensor<EQ_AM2320, true>::readHTSensor_(float &humidity,
                                                float &temperature) {
  if (__htSensor.read() == AM232X_OK) {
    humidity = __htSensor.humidity;
    temperature = __htSensor.temperature;
  }
}
#endif // (EQ_HT_SENSOR_TYPE == EQ_AM2320)
