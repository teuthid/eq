
#include "eq_eeprom.h"

const PROGMEM uint8_p EqEeprom::paramSizes_[] = {
    // including a size of the marker
    2, // LightSensorThreshold
    2, // HtSensorInterval
    2, // HtSensorHumidityThreshold
    2, // HtSensorTemperatureThreshold
    2, // HtIndexType
    3, // OverdriveStep
    2, // FanPwmInterval
    2, // FanPwmMin
    2, // FanPwmMax
    2, // FanPwmOverdrive
    2, // FanPwmStepMode
    2, // AlertOnZeroSpeed
    3, // BacklighTime
    2, // OverheatingCounter
};

void EqEeprom::init(const bool &reset) {
  eeprom_busy_wait();
  uint8_t __value = EEPROM.read(startAddress);
  if (reset || (__value != marker_)) {
    for (uint16_t __i = startAddress + 1; __i <= endAddress; __i++)
      EEPROM.update(__i, 0);
    EEPROM.update(startAddress, marker_);
  }
}

uint16_t EqEeprom::paramAddress_(ParameterId id) {
  uint16_t __address = startAddress + 1;
  for (uint8_t __i = 0; __i < id; __i++)
    __address += paramSizes_[__i];
  return __address;
}
