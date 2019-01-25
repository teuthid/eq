/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

// Based on the code from
// Arduino Library for Maxim Temperature Integrated Circuits
// Authors:
// Miles Burton <miles@mnetcs.com>,  Tim Newsome <nuisance@casualhacker.net>,
// Guil Barros <gfbarros@bappos.com>, Rob Tillaart <rob.tillaart@gmail.com>

#ifdef __STM32F1__
#include <OneWireSTM.h>
#else
#include <OneWire.h>
#endif

#include "eq_ht_sensor.h"

// OneWire commands
#define STARTCONVO 0x44
#define READSCRATCH 0xBE
#define WRITESCRATCH 0x4E

// Scratchpad locations
#define TEMP_LSB 0
#define TEMP_MSB 1
#define SCRATCHPAD_CRC 8

// Device resolution
#define TEMP_9_BIT 0x1F  //  9 bit
#define TEMP_10_BIT 0x3F // 10 bit
#define TEMP_11_BIT 0x5F // 11 bit
#define TEMP_12_BIT 0x7F // 12 bit

class EqDS18B20 {
public:
  typedef uint8_t DeviceAddress[8];
  typedef uint8_t ScratchPad[9];

  EqDS18B20(const uint8_t &sensorPin) : wire_(sensorPin) {}
  bool begin();
  bool isConnected();
  bool readTemperature(fixed_t &temperature);
  bool setResolution(const uint8_t &resolution);

private:
  DeviceAddress deviceAddress_ = {0};
  OneWire wire_;
  uint16_t waitMillis_ = 0;

  bool isAllZeros_(const ScratchPad &scratchPad) const;
  bool isConnected_(ScratchPad &scratchPad);
  bool isConversionComplete_() { return (wire_.read_bit() == 1); }
  bool readScratchPad_(ScratchPad &scratchPad);
  void requestTemperature_();
};

bool EqDS18B20::begin() {
  wire_.reset_search();
  wire_.search(deviceAddress_);
  return (wire_.crc8(deviceAddress_, 7) == deviceAddress_[7]);
}

bool EqDS18B20::isAllZeros_(const ScratchPad &scratchPad) const {
  for (uint8_t i = 0; i < sizeof(scratchPad); i++)
    if (scratchPad[i] != 0)
      return false;
  return true;
}

bool EqDS18B20::isConnected_(ScratchPad &scratchPad) {
  if (!readScratchPad_(scratchPad))
    return false;
  if (isAllZeros_(scratchPad))
    return false;
  if (!(wire_.crc8(scratchPad, 8) == scratchPad[SCRATCHPAD_CRC]))
    return false;
  return true;
}

bool EqDS18B20::isConnected() {
  ScratchPad scratchPad;
  return isConnected_(scratchPad);
}

bool EqDS18B20::readScratchPad_(ScratchPad &scratchPad) {
  if (wire_.reset() == 0)
    return false;
  wire_.select(deviceAddress_);
  wire_.write(READSCRATCH);
  for (uint8_t __i = 0; __i < sizeof(scratchPad); __i++) {
    scratchPad[__i] = wire_.read();
  }
  return (wire_.reset() == 1);
}

void EqDS18B20::requestTemperature_() {
  wire_.reset();
  wire_.skip();
  wire_.write(STARTCONVO, 0);
  unsigned long __start = millis();
  while (!isConversionComplete_() && (millis() - waitMillis_ < __start))
    yield();
}

bool EqDS18B20::readTemperature(fixed_t &temperature) {
  requestTemperature_();
  ScratchPad scratchPad;
  if (!isConnected_(scratchPad))
    return false;
  temperature = ((((int16_t)scratchPad[TEMP_MSB]) << 11) |
                 (((int16_t)scratchPad[TEMP_LSB]) << 3)) *
                0.0078125;
  return true;
}

bool EqDS18B20::setResolution(const uint8_t &resolution) {
  if (!isConnected())
    return false;
  wire_.reset();
  wire_.select(deviceAddress_);
  wire_.write(WRITESCRATCH);
  // two dummy values for LOW & HIGH ALARM
  wire_.write(0);
  wire_.write(100);
  switch (constrain(resolution, 9, 12)) {
  case 12:
    wire_.write(TEMP_12_BIT);
    waitMillis_ = 750;
    break;
  case 11:
    wire_.write(TEMP_11_BIT);
    waitMillis_ = 375;
    break;
  case 10:
    wire_.write(TEMP_10_BIT);
    waitMillis_ = 188;
    break;
  case 9:
  default:
    wire_.write(TEMP_9_BIT);
    waitMillis_ = 94;
    break;
  }
  return (wire_.reset() == 1);
}

EqDS18B20 __itSensor(EqConfig::itSensorPin);

// specializations for DS18B20 (internal sensor)

template <> bool EqItSensor::initHtSensor_() {
  if (__itSensor.begin())
    return __itSensor.setResolution(9);
  return false;
}

template <>
void EqItSensor::readHTSensor_(fixed_t &humidity, fixed_t &temperature) {
  __itSensor.readTemperature(temperature);
}

// specializations for DS18B20 (external sensor)
// TO DO
