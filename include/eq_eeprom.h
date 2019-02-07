/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#ifndef __EQ_EEPROM_H__
#define __EQ_EEPROM_H__

#include <EEPROM.h>
#include <PGMWrap.h>

#include "eq_interrupt_lock.h"

class EqEeprom final {
public:
  static constexpr uint16_t startAddress = 0x00;
  static constexpr uint16_t endAddress = 0xFF;

  EqEeprom(const EqEeprom &) = delete;
  EqEeprom(EqEeprom &&) = delete;
  void operator=(const EqEeprom &) = delete;

  static void init(const bool &reset = false);
  static void show();

  enum ParameterId : uint8_t {
    LastWatchdogPoint = 0,
    LightSensorThreshold = 1,
    HtSensorInterval = 2,
    HtSensorHumidityThreshold = 3,
    HtSensorTemperatureThreshold = 4,
    HtSensorHumidityCorrection = 5,    // round(hc * 10)
    HtSensorTemperatureCorrection = 6, // round(tc * 10)
    HtIndexType = 7,
    OverdriveStep = 8,
    FanPwmInterval = 9,
    FanPwmMin = 10,
    FanPwmMax = 11,
    FanPwmOverdrive = 12,
    FanPwmStepMode = 13,
    AlertOnZeroSpeed = 14,
    BacklighTime = 15,
    OverheatingCounter = 16,
    LedStatusMode = 17,
    BlowingEnabled = 18,
    BlowingInterval = 19,
    BlowingTime = 20,
  };

  template <typename T>
  static T readValue(ParameterId id, const T &defaultValue);
  template <typename T> static void writeValue(ParameterId id, const T &value);

private:
  EqEeprom();

  static constexpr uint8_t marker_ = 0xA5;
  static const PROGMEM uint8_p paramSizes_[];
  static uint16_t paramAddress_(ParameterId id);
};

// [address] <= value
// [address + sizeof(value)] <= marker

template <typename T>
T EqEeprom::readValue(ParameterId id, const T &defaultValue) {
  T __value = defaultValue;
  uint16_t __address = paramAddress_(id);
  if (EEPROM.read(__address + paramSizes_[id] - 1) == marker_)
    EEPROM.get(__address, __value);
  return __value;
}

template <typename T>
void EqEeprom::writeValue(ParameterId id, const T &value) {
  uint16_t __address = paramAddress_(id);
  EEPROM.put(__address, value);
  EEPROM.update(__address + paramSizes_[id] - 1, marker_);
}

#endif // __EQ_EEPROM_H__
