/*
   eq - transcendental fan controller ;)
   Copyright (c) 2017-2018 Mariusz Przygodzki
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
    LightSensorThreshold = 0,
    HtSensorInterval = 1,
    HtSensorHumidityThreshold = 2,
    HtSensorTemperatureThreshold = 3,
    HtIndexType = 4,
    OverdriveStep = 5,
    FanPwmInterval = 6,
    FanPwmMin = 7,
    FanPwmMax = 8,
    FanPwmOverdrive = 9,
    FanPwmStepMode = 10,
    AlertOnZeroSpeed = 11,
    BacklighTime = 12,
    OverheatingCounter = 13,
    LedStatusMode = 14
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
  EqInterruptLock __lock;
  EEPROM.put(__address, value);
  EEPROM.update(__address + paramSizes_[id] - 1, marker_);
}

#endif // __EQ_EEPROM_H__
