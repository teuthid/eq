/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#ifndef __EQ_HT_SENSOR_H__
#define __EQ_HT_SENSOR_H__

#include "eq_collector.h"
#include "eq_config.h"
#include "eq_display.h"

template <uint8_t Model, bool IsInternal> class EqHtSensor;
using EqItSensor = EqHtSensor<EQ_DS18B20, true>;

template <uint8_t Model, bool IsInternal = false> class EqHtSensor {
  friend EqHtSensor<EQ_HT_SENSOR_TYPE> &eqHtSensor();
  friend EqItSensor &eqItSensor();

public:
  static constexpr bool HumidityOn = !(IsInternal || (Model == EQ_DS18B20));

  EqHtSensor(const EqHtSensor &) = delete;
  EqHtSensor(EqHtSensor &&) = delete;
  void operator=(const EqHtSensor &) = delete;

  bool init();
  bool read();

  constexpr fixed_t humidity() const {
    return HumidityOn ? humidityCollector_.average() : 0;
  }
  constexpr fixed_t temperature() const {
    return temperatureCollector_.average();
  }
  constexpr fixed_t lastHumidity() const {
    return HumidityOn ? humidityCollector_.last() : 0;
  }
  constexpr long lastHumidityAsLong() const {
    return HumidityOn ? humidityCollector_.lastAsLong() : 0;
  }
  constexpr fixed_t lastTemperature() const {
    return temperatureCollector_.last();
  }
  constexpr long lastTemperatureAsLong() const {
    return temperatureCollector_.lastAsLong();
  }
  constexpr int8_t trendHumidity() const {
    return HumidityOn ? humidityCollector_.trend() : 0;
  }
  constexpr int8_t trendTemperature() const {
    return temperatureCollector_.trend();
  }

  uint8_t index() const;

private:
  EqCollector<(HumidityOn ? EqConfig::htSensorCollectorSize : 1)>
      humidityCollector_;
  EqCollector<EqConfig::htSensorCollectorSize> temperatureCollector_;
  static EqHtSensor instance_;

  constexpr EqHtSensor() {}

  // needs specializations:
  bool initHtSensor_();
  bool readHTSensor_(fixed_t &humidity, fixed_t &temperature);

  void setAlert_() const {
    EqConfig::setAlert(IsInternal ? EqAlertType::ItSensor
                                  : (HumidityOn ? EqAlertType::HtSensor
                                                : EqAlertType::TempSensor));
  }

  int8_t indexH_() const;
  int8_t indexT_() const;
};

inline EqHtSensor<EQ_HT_SENSOR_TYPE> &eqHtSensor() {
  return EqHtSensor<EQ_HT_SENSOR_TYPE>::instance_;
}
inline EqItSensor &eqItSensor() { return EqItSensor::instance_; }

template <uint8_t Model, bool IsInternal>
bool EqHtSensor<Model, IsInternal>::init() {
  const char *__s =
      IsInternal
          ? EqConfig::alertAsString(EqAlertType::ItSensor)
          : EqConfig::alertAsString(HumidityOn ? EqAlertType::HtSensor
                                               : EqAlertType::TempSensor);
#ifdef EQ_DEBUG
  Serial.print(__s);
#endif
  eqDisplay().showMessage(__s);
  if (!initHtSensor_()) {
    setAlert_();
    return false; // failed initialization of sensor
  }
  if (HumidityOn)
    humidityCollector_.setAcceptableValueRange(EqConfig::htSensorHumidityMin,
                                               EqConfig::htSensorHumidityMax);
  if (!IsInternal)
    temperatureCollector_.setAcceptableValueRange(
        EqConfig::htSensorTemperatureMin, EqConfig::htSensorTemperatureMax);
  uint8_t __c = 0;
  for (uint8_t __i = 0; __i < EqConfig::htSensorCollectorSize; __i++) {
    if (read())
      __c++;
    // necessary for sequential reading of the sensor:
    delay(IsInternal ? EqConfig::itSensorSamplingPeriod
                     : EqConfig::htSensorSamplingPeriod);
  }
  if (__c == 0) {
    setAlert_();
    return false; // problem with reading data from the sensor
  }
  return true;
}

template <uint8_t Model, bool IsInternal>
bool EqHtSensor<Model, IsInternal>::read() {
  fixed_t __h = 0;
  fixed_t __t = 0;
  if (!readHTSensor_(__h, __t))
    return false; // incorrect reading of humidity or temperature
  if (HumidityOn)
    __h += EqConfig::htSensorHumidityCorrection();
  if (!IsInternal)
    __t += EqConfig::htSensorTemperatureCorrection();
  bool __ctrl = HumidityOn ? humidityCollector_.add(__h) : true;
  __ctrl = __ctrl && temperatureCollector_.add(__t);
  if (HumidityOn)
    __ctrl = __ctrl &&
             (humidityCollector_.deviation() < EqConfig::sensorMaxDeviation);
  if (!IsInternal)
    __ctrl = __ctrl &&
             (temperatureCollector_.deviation() < EqConfig::sensorMaxDeviation);
  return __ctrl; // false if incorrect data reading
}

template <uint8_t Model, bool IsInternal>
int8_t EqHtSensor<Model, IsInternal>::indexH_() const {
  if (HumidityOn) {
    long __h = fixed_to_long(10 * humidity()) + trendHumidity(),
         __ht = 10L * EqConfig::htSensorHumidityThreshold();
    if (__h < __ht)
      return map(__h, 10L * EqConfig::htSensorHumidityMin, __ht, -100, 0);
    else
      return map(__h, __ht, 10L * EqConfig::htSensorHumidityMax, 0, 100);
  } else
    return 0; // humidity measure is off
}

template <uint8_t Model, bool IsInternal>
int8_t EqHtSensor<Model, IsInternal>::indexT_() const {
  long __t = fixed_to_long(10 * temperature()) + trendTemperature(),
       __tt = 10L * EqConfig::htSensorTemperatureThreshold();
  if (__t < __tt)
    return map(__t, 10L * EqConfig::htSensorTemperatureMin, __tt, -100, 0);
  else
    return map(__t, __tt, 10L * EqConfig::htSensorTemperatureMax, 0, 100);
}

template <uint8_t Model, bool IsInternal>
uint8_t EqHtSensor<Model, IsInternal>::index() const {
  int8_t __h = indexH_(), __t = indexT_();
  switch (EqConfig::htIndexType()) {
  case EqHtIndexType::MoreTemperatureSensitive:
    return constrain(max(__h + __t, __t), 0, 100);
  case EqHtIndexType::LessTemperatureSensitive:
    return constrain(max(__h, __t), 0, 100);
  case EqHtIndexType::OnlyHumidity:
    return constrain(__h, 0, 100);
  case EqHtIndexType::OnlyTemperature:
    return constrain(__t, 0, 100);
  case EqHtIndexType::Default:
  default:
    return constrain(__h + __t, 0, 100);
  }
}

#endif // __EQ_HT_SENSOR_H__
