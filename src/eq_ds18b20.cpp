/*
   eq - transcendental fan controller ;)
   Copyright (c) 2017-2018 Mariusz Przygodzki
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

#include "eq_fixedpoints.h"
#include "eq_ht_sensor.h"

// OneWire commands
#define STARTCONVO 0x44
#define READSCRATCH 0xBE
#define WRITESCRATCH 0x4E

// Scratchpad locations
#define TEMP_LSB 0
#define TEMP_MSB 1

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
  bool readTemperature(fixed_t &temperature, const uint16_t &waitMillis);
  void setResolution(const uint8_t &resolution);

private:
  DeviceAddress deviceAddress_ = {0};
  OneWire wire_;

  bool readScratchPad_(ScratchPad &scratchPad, const uint8_t &fields);
  bool isConversionComplete_() { return (wire_.read_bit() == 1); }
  void requestTemperature_(const uint16_t &waitMillis);
};

bool EqDS18B20::begin() {
  wire_.reset_search();
  wire_.search(deviceAddress_);
  return (wire_.crc8(deviceAddress_, 7) == deviceAddress_[7]);
}

bool EqDS18B20::readScratchPad_(ScratchPad &scratchPad, const uint8_t &fields) {
  if (wire_.reset() == 0)
    return false;
  wire_.select(deviceAddress_);
  wire_.write(READSCRATCH);
  for (uint8_t i = 0; i < fields; i++) {
    scratchPad[i] = wire_.read();
  }
  return (wire_.reset() == 1);
}

void EqDS18B20::requestTemperature_(const uint16_t &waitMillis) {
  wire_.reset();
  wire_.skip();
  wire_.write(STARTCONVO, 0);
}

bool EqDS18B20::readTemperature(fixed_t &temperature,
                                const uint16_t &waitMillis) {
  requestTemperature_(waitMillis);
  ScratchPad scratchPad;
  if (!readScratchPad_(scratchPad, 2))
    return false;
  fixed_t __t =
      ((((int16_t)scratchPad[TEMP_MSB]) << 8) | scratchPad[TEMP_LSB]) * 0.0625;
  if ((__t < -55) || (__t > 125))
    return false;
  temperature = __t;
  return true;
}

void EqDS18B20::setResolution(const uint8_t &resolution) {
  wire_.reset();
  wire_.select(deviceAddress_);
  wire_.write(WRITESCRATCH);
  // two dummy values for LOW & HIGH ALARM
  wire_.write(0);
  wire_.write(100);
  switch (constrain(resolution, 9, 12)) {
  case 12:
    wire_.write(TEMP_12_BIT);
    break;
  case 11:
    wire_.write(TEMP_11_BIT);
    break;
  case 10:
    wire_.write(TEMP_10_BIT);
    break;
  case 9:
  default:
    wire_.write(TEMP_9_BIT);
    break;
  }
  wire_.reset();
}

EqDS18B20 __itSensor(EqConfig::itSensorPin);

// specializations for DS18B20 (internal sensor)

template <> bool EqItSensor::initHtSensor_() {
  if (__itSensor.begin()) {
    __itSensor.setResolution(9);
    fixed_t __t;
    return __itSensor.readTemperature(__t, 94);
  } else
    return false;
}

template <>
void EqItSensor::readHTSensor_(fixed_t &humidity, fixed_t &temperature) {
  __itSensor.readTemperature(temperature, 94); // blocking !!!
}

// specializations for DS18B20 (external sensor)
// TO DO
