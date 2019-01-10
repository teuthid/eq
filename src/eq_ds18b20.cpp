/*
   eq - transcendental fan controller ;)
   Copyright (c) 2017-2018 Mariusz Przygodzki
*/

// Based on the code from
// Arduino Library for Maxim Temperature Integrated Circuits
// Authors:
// Miles Burton <miles@mnetcs.com>,  Tim Newsome <nuisance@casualhacker.net>,
// Guil Barros <gfbarros@bappos.com>, Rob Tillaart <rob.tillaart@gmail.com>

#include <OneWire.h>

// OneWire commands
#define STARTCONVO 0x44
#define READSCRATCH 0xBE
#define WRITESCRATCH 0x4E

class EqDS18B20 {
public:
  typedef uint8_t DeviceAddress[8];
  typedef uint8_t ScratchPad[9];

  EqDS18B20(const uint8_t &sensorPin) : wire_(sensorPin) {}
  bool begin();
  void readScratchPad(ScratchPad &scratchPad, const uint8_t &fields);
  bool isConversionComplete() { return (wire_.read_bit() == 1); }

private:
  DeviceAddress deviceAddress_ = {0};
  OneWire wire_;
};

bool EqDS18B20::begin() {
  wire_.reset_search();
  wire_.search(deviceAddress_);
  return (wire_.crc8(deviceAddress_, 7) == deviceAddress_[7]);
}

void EqDS18B20::readScratchPad(ScratchPad &scratchPad, const uint8_t &fields) {
  wire_.reset();
  wire_.select(deviceAddress_);
  wire_.write(READSCRATCH);
  for (uint8_t i = 0; i < fields; i++) {
    scratchPad[i] = wire_.read();
  }
  wire_.reset();
}
