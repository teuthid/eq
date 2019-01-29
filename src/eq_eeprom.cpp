/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#include "eq_eeprom.h"
#include "eq_config.h"

const PROGMEM uint8_p EqEeprom::paramSizes_[] = {
    // including a size of the marker
    2, // LightSensorThreshold (uint8_t)
    2, // HtSensorInterval (uint8_t)
    2, // HtSensorHumidityThreshold (uint8_t)
    2, // HtSensorTemperatureThreshold (uint8_t)
    2, // HtIndexType (uint8_t)
    3, // OverdriveStep (uint16_t)
    2, // FanPwmInterval (uint8_t)
    2, // FanPwmMin (uint8_t)
    2, // FanPwmMax (uint8_t)
    2, // FanPwmOverdrive (uint8_t)
    2, // FanPwmStepMode (bool)
    2, // AlertOnZeroSpeed (bool)
    3, // BacklighTime (uint16_t)
    2, // OverheatingCounter (uint8_t)
    2, // LedStatusMode (uint8_t)
    2, // BlowingEnabled (bool)
    2, // BlowingInterval (uint8_t)
};

void EqEeprom::init(const bool &reset) {
  eeprom_busy_wait();
  uint8_t __value = EEPROM.read(startAddress);
  if (reset || (__value != marker_)) {
    EQ_INTERRUPT_LOCK
    for (uint16_t __i = startAddress + 1; __i <= endAddress; __i++)
      EEPROM.update(__i, 0);
    EEPROM.update(startAddress, marker_);
  }
}

void EqEeprom::show() {
  uint16_t __size = 0;
  for (uint8_t __i = 0; __i < sizeof(paramSizes_); __i++)
    __size += paramSizes_[__i];
  Serial.print(F("EEPROM: "));
  for (uint16_t __i = startAddress; __i <= startAddress + __size; __i++) {
    Serial.print(EEPROM.read(__i), HEX);
    Serial.write(0x20);
  }
  Serial.println();
}

uint16_t EqEeprom::paramAddress_(ParameterId id) {
  uint16_t __address = startAddress + 1;
  for (uint8_t __i = 0; __i < id; __i++)
    __address += paramSizes_[__i];
  return __address;
}
