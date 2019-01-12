/*
   eq - transcendental fan controller ;)
   Copyright (c) 2017-2018 Mariusz Przygodzki
*/

#ifndef __EQ_CONFIG_H__
#define __EQ_CONFIG_H__

#include "Arduino.h"

#define EQ_DEBUG

#define EQ_DS18B20 0
#define EQ_DHT11 1
#define EQ_DHT21 2 // AM2301
#define EQ_DHT22 3 // AM2302
#define EQ_HTU21D 4
#define EQ_AM2320 5
#define EQ_HT_SENSOR_TYPE EQ_AM2320

#define EQ_NO_DISPLAY 0
#define EQ_LCD_1602 1 // with I2C only!
#define EQ_LCD_2004 2 // with I2C only!
#define EQ_DISPLAY_TYPE EQ_LCD_1602

enum class EqHtIndexType : uint8_t {
  Default = 0,
  MoreTemperatureSensitive = 1,
  LessTemperatureSensitive = 2,
  OnlyHumidity = 3,
  OnlyTemperature = 4
};

enum class EqAlertType : uint8_t {
  None = 0,
  Display = 1,
  Fan = 2,
  HtSensor = 3,
  LightSensor = 4,
  TempSensor = 5, // temperature sensor only
  ItSensor = 6,   // internal temperature sensor
  Overheating = 7
};

class EqConfig final {
public:
  // hardware configuration
  static constexpr uint8_t ledHeartbeatPin = 13;
  static constexpr uint8_t ledAlertPin = 4;
  static constexpr uint8_t lcdI2CAddress = 0x27;
  static constexpr uint8_t lightSensorPin = A0;
  static constexpr uint8_t htSensorPin = 8; // if I2C is not used
  static constexpr uint8_t am2320I2CAddress = 0x5C;
  static constexpr uint8_t buttonOverdrivePin = 2;
  static constexpr uint8_t buttonBacklightPin = 5;
  static constexpr uint8_t fanPwmPin = 9;
  static constexpr uint8_t fanTachometerPin = 3;
  static constexpr uint8_t fanTachometerControlPin = A1;
  static constexpr uint8_t itSensorPin = A2;

  // non-configurable parameters
  static constexpr uint8_t sensorMaxDeviation = 30;        // in percents
  static constexpr uint16_t htSensorSamplingPeriod = 2000; // in milliseconds
  static constexpr uint16_t itSensorSamplingPeriod = 1000; // in milliseconds
  static constexpr uint8_t itSensorMaxTemperature = 50;
  static constexpr uint8_t itSensorInterval = 1; // in seconds
#ifdef EQ_DEBUG
  static constexpr uint8_t debugInterval = 5; // in seconds
#endif

  EqConfig(const EqConfig &) = delete;
  EqConfig(EqConfig &&) = delete;
  void operator=(const EqConfig &) = delete;

  static bool init();
  static void reset(const bool &cleanEeprom = false);
  static void sleep();
  template <typename T>
  static void printValue(const __FlashStringHelper *description,
                         const T &value) {
    Serial.print(description);
    Serial.print(value);
  }
  static void show();

  // alert
  static constexpr bool alertOnZeroSpeedDefault = true;
  static EqAlertType alert() { return alert_; }
  static void setAlert(const EqAlertType &value);
  static void resetAlert(const EqAlertType &value);
  static bool anyAlert() { return (alert_ != EqAlertType::None); }
  static bool isAlertOnZeroSpeed();
  static void setAlertOnZeroSpeed(const bool &enabled = true);
  static const char *alertAsString(const EqAlertType &alert);
  static void showAlert(const EqAlertType &alert);

  // overheating
  static constexpr uint8_t maxCountOverheating = 3;
  static bool overheating();
  static void registerOverheating();
  static void cancelOverheating();

  // light sensor
  static constexpr bool lightSensorIsAnalog = true;
  static constexpr uint8_t lightSensorCollectorSize = 5;
  static constexpr uint8_t lightSensorThresholdDefault = 40; // in percents
  static constexpr uint8_t lightSensorThresholdMin = 20;
  static constexpr uint8_t lightSensorThresholdMax = 80;
  static uint8_t lightSensorThreshold();
  static void setLightSensorThreshold(const uint8_t &value);

  // HT sensor
  static constexpr uint8_t htSensorCollectorSize = 5;
  static constexpr uint8_t htSensorIntervalDefault = 3; // in seconds
  static constexpr uint8_t htSensorIntervalMin =
      max(1, htSensorSamplingPeriod / 1000);
  static constexpr uint8_t htSensorIntervalMax = 60;
  static constexpr uint8_t htSensorHumidityThresholdDefault = 45; // in percents
  static constexpr uint8_t htSensorHumidityThresholdMin = 20;
  static constexpr uint8_t htSensorHumidityThresholdMax = 80;
  static constexpr uint8_t htSensorHumidityMin = 5;
  static constexpr uint8_t htSensorHumidityMax = 95;
  static constexpr uint8_t htSensorTemperatureThresholdDefault = 24; // in *C
  static constexpr uint8_t htSensorTemperatureThresholdMin = 15;
  static constexpr uint8_t htSensorTemperatureThresholdMax = 30;
  static constexpr uint8_t htSensorTemperatureMin = 5;
  static constexpr uint8_t htSensorTemperatureMax = itSensorMaxTemperature;
  static uint8_t htSensorInterval();
  static void setHtSensorInterval(const uint8_t &value);
  static uint8_t htSensorHumidityThreshold();
  static void setHtSensorHumidityThreshold(const uint8_t &value);
  static uint8_t htSensorTemperatureThreshold();
  static void setHtSensorTemperatureThreshold(const uint8_t &value);
  static EqHtIndexType htIndexType();
  static void setHtIndexType(const EqHtIndexType &value);

  // overdrive
  static constexpr uint16_t overdriveMaxTime = 3599; // in seconds
  static constexpr uint16_t overdriveStepDefault = 10;
  static constexpr uint16_t overdriveStepMin = 10;
  static constexpr uint16_t overdriveStepMax = 600;
  static uint16_t overdriveTime() { return overdriveTime_; }
  static void setOverdriveTime(const uint16_t &value);
  static void decreaseOverdriveTime(const uint16_t &value = 1);
  static void increaseOverdriveTime(const uint16_t &value = 1);
  static uint16_t overdriveStep();
  static void setOverdriveStep(const uint16_t &value);

  // fan PWM
  static constexpr uint8_t fanPwmIntervalDefault = 2; // in seconds
  static constexpr uint8_t fanPwmIntervalMin = 1;
  static constexpr uint8_t fanPwmIntervalMax = 5;
  static constexpr uint8_t fanPwmMinDefault = 10; // in percents
  static constexpr uint8_t fanPwmMaxDefault = 100;
  static constexpr uint8_t fanPwmOverdriveDefault = fanPwmMaxDefault;
  static constexpr uint32_t fanPwmCycle = 100000; // in microseconds
  static constexpr bool fanPwmStepModeDefault = false;
  static uint8_t fanPwmInterval();
  static void setFanPwmInterval(const uint8_t &value);
  static uint8_t fanPwmMin();
  static void setFanPwmMin(const uint8_t &value);
  static uint8_t fanPwmMax();
  static void setFanPwmMax(const uint8_t &value);
  static uint8_t fanPwmOverdrive();
  static void setFanPwmOverdrive(const uint8_t &value);
  static bool isFanPwmStepModeEnabled();
  static void setFanPwmStepMode(const bool &enabled = true);

  // tachometer
  static bool isFanTachometerEnabled();

  // display backlight
  static constexpr uint16_t backlightTimeDefault = 60; // in seconds
  static constexpr uint16_t backlightTimeMax = 300;
  static constexpr uint16_t backlightTimeMin = 5;
  static uint16_t backlightTime();
  static void setBacklighTime(const uint16_t &value);
  static void setBacklight(bool enabled = true);
  static bool backlight() { return (backlightTimeCounter_ > 0); }
  static void decreaseBacklightTimeCounter();

  // buttons
  static constexpr uint8_t buttonReadInterval = 10; // in milliseconds
  static constexpr uint32_t buttonDebounceTime = 100;
  static constexpr uint32_t buttonLongPressedTime = 5000;

private:
  EqConfig() {}
  static EqAlertType alert_;
  static uint16_t overdriveTime_;
  static uint16_t backlightTimeCounter_;
};

#endif // __EQ_CONFIG_H__
