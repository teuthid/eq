
#ifndef __EQ_HT_SENSOR_H__
#define __EQ_HT_SENSOR_H__

#include "eq_collector.h"
#include "eq_config.h"
#include "eq_display.h"

template <uint8_t Model, bool IsInternal = false> class EqHtSensor {
public:
  EqHtSensor(const uint8_t &sensorPin = EqConfig::htSensorPin) {}

  static constexpr bool HumidityOn = !(IsInternal || (Model == EQ_DS18B20));

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
  constexpr fixed_t lastTemperature() const {
    return temperatureCollector_.last();
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

  // needs specializations:
  uint16_t samplingPeriod_() const; // in milliseconds
  bool initHtSensor_();
  void readHTSensor_(fixed_t &humidity, fixed_t &temperature);

  void setAlert() {
    EqConfig::setAlert(IsInternal ? EqAlertType::ItSensor
                                  : (HumidityOn ? EqAlertType::HtSensor
                                                : EqAlertType::TempSensor));
  }

  int8_t indexH_() const;
  int8_t indexT_() const;
};

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
  eqDisplay.showMessage(__s);
  if (!initHtSensor_()) {
    setAlert();
    return false;
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
    delay(samplingPeriod_());
  }
  if (__c == 0) {
    setAlert();
    return false;
  }
  return true;
}

template <uint8_t Model, bool IsInternal>
bool EqHtSensor<Model, IsInternal>::read() {
  fixed_t __h = 0, __t = 0;
  readHTSensor_(__h, __t);
  bool __ctrl = HumidityOn ? humidityCollector_.add(__h) : true;
  __ctrl = __ctrl && temperatureCollector_.add(__t);
  if (HumidityOn)
    __ctrl = __ctrl &&
             (humidityCollector_.deviation() < EqConfig::sensorMaxDeviation);
  if (!IsInternal)
    __ctrl = __ctrl &&
             (temperatureCollector_.deviation() < EqConfig::sensorMaxDeviation);
  return __ctrl;
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

extern EqHtSensor<EQ_HT_SENSOR_TYPE> eqHtSensor;
extern EqHtSensor<EQ_DS18B20, true> eqItSensor;

/*
  sensor specializations:
*/

// --- DHT11 ------------------------------------------------------------------
/*
if (EQ_HT_SENSOR_TYPE == EQ_DHT11)
#include <DHT.h>

template <> struct __EqHtSensorObject<EQ_DHT11> { typedef DHT Type; };
template <>
EqHtSensor<EQ_DHT11, true>::EqHtSensor(
    const uint8_t &sensorPin = EqConfig::htSensorPin)
    : sensor_(sensorPin, DHT11) {
  sensor_.begin();
}
template <> uint16_t EqHtSensor<EQ_DHT11, true>::samplingPeriod_() const {
  return 2000;
}
template <>
void EqHtSensor<EQ_DHT11, true>::readHTSensor_(float &humidity,
                                               float &temperature) {
  humidity = sensor_.readHumidity();
  temperature = sensor_.readTemperature();
}
*/
// ----------------------------------------------------------------------------

// --- DHT21 ------------------------------------------------------------------
/*
#elif (EQ_HT_SENSOR_TYPE == EQ_DHT21)
#include <DHT.h>

template <> struct __EqHtSensorObject<EQ_DHT21> { typedef DHT Type; };
template <>
EqHtSensor<EQ_DHT21, true>::EqHtSensor(
    const uint8_t &sensorPin = EqConfig::htSensorPin)
    : sensor_(sensorPin, DHT21) {
  sensor_.begin();
}
template <> uint16_t EqHtSensor<EQ_DHT21, true>::samplingPeriod_() const {
  return 2000;
}
template <>
void EqHtSensor<EQ_DHT21, true>::readHTSensor_(float &humidity,
                                               float &temperature) {
  humidity = sensor_.readHumidity();
  temperature = sensor_.readTemperature();
}
*/
// ----------------------------------------------------------------------------

// --- DHT22 ------------------------------------------------------------------
/*
#elif (EQ_HT_SENSOR_TYPE == EQ_DHT22)
#include <DHT.h>

template <> struct __EqHtSensorObject<EQ_DHT22> { typedef DHT Type; };
template <>
EqHtSensor<EQ_DHT22, true>::EqHtSensor(
    const uint8_t &sensorPin = EqConfig::htSensorPin)
    : sensor_(sensorPin, DHT22) {
  sensor_.begin();
}
template <> uint16_t EqHtSensor<EQ_DHT22, true>::samplingPeriod_() const {
  return 2000;
}
template <>
void EqHtSensor<EQ_DHT22, true>::readHTSensor_(float &humidity,
                                               float &temperature) {
  humidity = sensor_.readHumidity();
  temperature = sensor_.readTemperature();
}
*/
// ----------------------------------------------------------------------------

// --- HTU21D -----------------------------------------------------------------
/*
#elif (EQ_HT_SENSOR_TYPE == EQ_HTU21D)
#include <HTU21D.h>
#include <Wire.h>

template <> struct __EqHtSensorObject<EQ_HTU21D> { typedef HTU21D Type; };
template <> uint16_t EqHtSensor<EQ_HTU21D, true>::samplingPeriod_() const {
  return 100;
}
template <> bool EqHtSensor<EQ_HTU21D, true>::initHtSensor_() {
  return sensor_.begin();
}
template <>
void EqHtSensor<EQ_HTU21D, true>::readHTSensor_(float &humidity,
                                                float &temperature) {
  humidity = sensor_.readHumidity();
  temperature = sensor_.readTemperature();
}
*/
// ----------------------------------------------------------------------------

#endif // __EQ_HT_SENSOR_H__
