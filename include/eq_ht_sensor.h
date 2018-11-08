
#ifndef __EQ_HT_SENSOR_H__
#define __EQ_HT_SENSOR_H__

#include "eq_collector.h"
#include "eq_config.h"

template <uint8_t SensorType> struct __EqHtSensorObject { typedef int Type; };

template <uint8_t SensorType, bool HumidityOn = true> class EqHtSensor {
public:
  constexpr EqHtSensor(const uint8_t &sensorPin = EqConfig::htSensorPin) {}

  constexpr bool humidityOn() const { return HumidityOn; }

  bool init(const bool &isITSensor = false);
  bool read(const bool &isITSensor = false);

  constexpr float humidity() const {
    return HumidityOn ? humidityCollector_.average() : 0;
  }
  constexpr float temperature() const {
    return temperatureCollector_.average();
  }
  constexpr float lastHumidity() const {
    return HumidityOn ? humidityCollector_.last() : 0;
  }
  constexpr float lastTemperature() const {
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
  typename __EqHtSensorObject<SensorType>::Type sensor_;
  EqCollector<(HumidityOn ? EqConfig::htSensorCollectorSize : 1)>
      humidityCollector_;
  EqCollector<EqConfig::htSensorCollectorSize> temperatureCollector_;

  bool initHtSensor_(); // { return true; }
  void readHTSensor_(float &humidity, float &temperature);

  void setAlert() {
    EqConfig::setAlert(HumidityOn ? EqAlertType::HtSensor
                                  : EqAlertType::TempSensor);
  }

  int8_t indexH_() const;
  int8_t indexT_() const;
};

template <uint8_t SensorType, bool HumidityOn>
bool EqHtSensor<SensorType, HumidityOn>::init(const bool &isITSensor) {
#ifdef EQ_DEBUG
  Serial.print(HumidityOn ? F("[HT Sensor] ") : F("[Temp Sensor] "));
#endif
  if (!initHtSensor_()) {
    setAlert();
    return false;
  }
  if (HumidityOn)
    humidityCollector_.setAcceptableValueRange(EqConfig::htSensorHumidityMin,
                                               EqConfig::htSensorHumidityMax);
  if (!isITSensor)
    temperatureCollector_.setAcceptableValueRange(
        EqConfig::htSensorTemperatureMin, EqConfig::htSensorTemperatureMax);
  uint8_t __c = 0;
  for (uint8_t __i = 0; __i < EqConfig::htSensorCollectorSize; __i++) {
    if (read())
      __c++;
    delay(500); // FIXME
  } 
  if (__c == 0) {
    setAlert();
    return false;
  }
  return true;
}

template <uint8_t SensorType, bool HumidityOn>
bool EqHtSensor<SensorType, HumidityOn>::read(const bool &isITSensor) {
  float __h = 0, __t = 0;
  readHTSensor_(__h, __t);
  if ((HumidityOn && isnan(__h)) || isnan(__t))
    return false;
  bool __ctrl = HumidityOn ? humidityCollector_.add(__h) : true;
  __ctrl = __ctrl && temperatureCollector_.add(__t);
  if (HumidityOn)
    __ctrl = __ctrl &&
             (humidityCollector_.deviation() < EqConfig::sensorMaxDeviation);
  if (!isITSensor)
    __ctrl = __ctrl &&
             (temperatureCollector_.deviation() < EqConfig::sensorMaxDeviation);
  return __ctrl;
}

template <uint8_t SensorType, bool HumidityOn>
int8_t EqHtSensor<SensorType, HumidityOn>::indexH_() const {
  if (HumidityOn) {
    long __h = round(10 * humidity()) + trendHumidity(),
         __ht = 10 * EqConfig::htSensorHumidityThreshold();
    if (__h < __ht)
      return map(__h, 10 * EqConfig::htSensorHumidityMin, __ht, -100, 0);
    else
      return map(__h, __ht, 10 * EqConfig::htSensorHumidityMax, 0, 100);
  } else
    return 0; // humidity measure is off
}

template <uint8_t SensorType, bool HumidityOn>
int8_t EqHtSensor<SensorType, HumidityOn>::indexT_() const {
  long __t = round(10 * temperature()) + trendTemperature(),
       __tt = 10 * EqConfig::htSensorTemperatureThreshold();
  if (__t < __tt)
    return map(__t, 10 * EqConfig::htSensorTemperatureMin, __tt, -100, 0);
  else
    return map(__t, __tt, 10 * EqConfig::htSensorTemperatureMax, 0, 100);
}

template <uint8_t SensorType, bool HumidityOn>
uint8_t EqHtSensor<SensorType, HumidityOn>::index() const {
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

/*
  sensors specializations:
*/

// --- AM2320 -----------------------------------------------------------------
#if (EQ_HT_SENSOR_TYPE == EQ_AM2320)
#include <Adafruit_AM2320.h>
#include <Adafruit_Sensor.h>

template <> struct __EqHtSensorObject<EQ_AM2320> {
  typedef Adafruit_AM2320 Type;
};
template <> bool EqHtSensor<EQ_AM2320, true>::initHtSensor_() {
  return sensor_.begin();
}
template <>
void EqHtSensor<EQ_AM2320, true>::readHTSensor_(float &humidity,
                                                float &temperature) {
  humidity = sensor_.readHumidity();
  temperature = sensor_.readTemperature();
}
// ----------------------------------------------------------------------------

// --- DHT11 ------------------------------------------------------------------
#elif (EQ_HT_SENSOR_TYPE == EQ_DHT11)
#include <DHT.h>

template <> struct __EqHtSensorObject<EQ_DHT11> { typedef DHT Type; };
template <>
EqHtSensor<EQ_DHT11, true>::EqHtSensor(
    const uint8_t &sensorPin = EqConfig::htSensorPin)
    : sensor_(sensorPin, DHT11) {
  sensor_.begin();
}
template <>
void EqHtSensor<EQ_DHT11, true>::readHTSensor_(float &humidity,
                                               float &temperature) {
  humidity = sensor_.readHumidity();
  temperature = sensor_.readTemperature();
}
// ----------------------------------------------------------------------------

// --- DHT21 ------------------------------------------------------------------
#elif (EQ_HT_SENSOR_TYPE == EQ_DHT21)
#include <DHT.h>

template <> struct __EqHtSensorObject<EQ_DHT21> { typedef DHT Type; };
template <>
EqHtSensor<EQ_DHT21, true>::EqHtSensor(
    const uint8_t &sensorPin = EqConfig::htSensorPin)
    : sensor_(sensorPin, DHT21) {
  sensor_.begin();
}
template <>
void EqHtSensor<EQ_DHT21, true>::readHTSensor_(float &humidity,
                                               float &temperature) {
  humidity = sensor_.readHumidity();
  temperature = sensor_.readTemperature();
}
// ----------------------------------------------------------------------------

// --- DHT22 ------------------------------------------------------------------
#elif (EQ_HT_SENSOR_TYPE == EQ_DHT22)
#include <DHT.h>

template <> struct __EqHtSensorObject<EQ_DHT22> { typedef DHT Type; };
template <>
EqHtSensor<EQ_DHT22, true>::EqHtSensor(
    const uint8_t &sensorPin = EqConfig::htSensorPin)
    : sensor_(sensorPin, DHT22) {
  sensor_.begin();
}
template <>
void EqHtSensor<EQ_DHT22, true>::readHTSensor_(float &humidity,
                                               float &temperature) {
  humidity = sensor_.readHumidity();
  temperature = sensor_.readTemperature();
}
// ----------------------------------------------------------------------------

// --- HTU21D -----------------------------------------------------------------
#elif (EQ_HT_SENSOR_TYPE == EQ_HTU21D)
#include <HTU21D.h>
#include <Wire.h>

template <> struct __EqHtSensorObject<EQ_HTU21D> { typedef HTU21D Type; };
template <> bool EqHtSensor<EQ_HTU21D, true>::initHtSensor_() {
  return sensor_.begin();
}
template <>
void EqHtSensor<EQ_HTU21D, true>::readHTSensor_(float &humidity,
                                                float &temperature) {
  humidity = sensor_.readHumidity();
  temperature = sensor_.readTemperature();
}
// ----------------------------------------------------------------------------

#else
#error "Unknown type of HT Sensor"
#endif

#endif // __EQ_HT_SENSOR_H__
