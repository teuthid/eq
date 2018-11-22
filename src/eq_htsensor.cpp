
#include "eq_ht_sensor.h"

/*
  sensor specializations:
*/

// --- AM2320 -----------------------------------------------------------------
#if (EQ_HT_SENSOR_TYPE == EQ_AM2320)
#include <AM232X.h>

template <> EqHtSensor<EQ_AM2320, true>::EqHtSensor(const uint8_t &sensorPin) {
  sensor_ = new AM232X;
}

template <> uint16_t EqHtSensor<EQ_AM2320, true>::samplingPeriod_() const {
  return 2000;
}

template <> bool EqHtSensor<EQ_AM2320, true>::initHtSensor_() {
  return (static_cast<AM232X *>(sensor_)->read() == AM232X_OK);
}

template <>
void EqHtSensor<EQ_AM2320, true>::readHTSensor_(float &humidity,
                                                float &temperature) {
  if (static_cast<AM232X *>(sensor_)->read() == AM232X_OK) {
    humidity = static_cast<AM232X *>(sensor_)->humidity;
    temperature = static_cast<AM232X *>(sensor_)->temperature;
  }
}
#endif // (EQ_HT_SENSOR_TYPE == EQ_AM2320)

