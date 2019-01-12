/*
   eq - transcendental fan controller ;)
   Copyright (c) 2017-2018 Mariusz Przygodzki
*/

// Based on the code from
// Library for AM232X Temperature and Humidity sensor
// Author: Rob Tillaart <rob.tillaart@gmail.com>

#if (EQ_HT_SENSOR_TYPE == EQ_AM2320)

#include <Wire.h>

#include "eq_ht_sensor.h"

class EqAM2320 {
public:
  constexpr EqAM2320() {}
  bool readHT(fixed_t &humidity, fixed_t &temperature);

private:
  static constexpr uint8_t i2cAddress_ = 0x5C;
  uint8_t buffer_[8] = {0};

  uint16_t crc16_() const;
  bool readRegister_();
};

uint16_t EqAM2320::crc16_() const {
  uint16_t __crc = 0xFFFF;
  for (uint8_t __i = 0; __i < 6; __i++) {
    __crc ^= buffer_[__i];
    for (uint8_t __j = 0; __j < 8; __j++) {
      if (__crc & 0x01) {
        __crc >>= 1;
        __crc ^= 0xA001;
      } else {
        __crc >>= 1;
      }
    }
  }
  return __crc;
}

bool EqAM2320::readRegister_() {
  // wake up the sensor - see 8.2
  Wire.beginTransmission(i2cAddress_);
  Wire.endTransmission();
  delayMicroseconds(800);

  // request the data
  Wire.beginTransmission(i2cAddress_);
  Wire.write(0x03);
  Wire.write(0x00);
  Wire.write(4);
  if (Wire.endTransmission() < 0)
    return false;
  delayMicroseconds(1500);

  // request 4 extra, 2 for cmd + 2 for CRC
  if (Wire.requestFrom(i2cAddress_, (uint8_t)8) != 8)
    return false;
  for (uint8_t __i = 0; __i < 8; __i++) {
    buffer_[__i] = Wire.read();
  }

  // CRC is LOW Byte first
  uint16_t __crc = buffer_[7] * 256 + buffer_[6];
  if (crc16_() != __crc) {
    return false; // read itself has wrong CRC
  }
  return true;
}

bool EqAM2320::readHT(fixed_t &humidity, fixed_t &temperature) {
  if (!readRegister_())
    return false;
  humidity = (buffer_[2] * 256 + buffer_[3]) * 0.1;
  temperature = (buffer_[4] & 0x7F) * 256 + buffer_[5] * 0.1;
  if (buffer_[4] & 0x80) {
    temperature = -temperature;
  }
  return true;
}

EqAM2320 __htSensor;

template <> bool EqHtSensor<EQ_AM2320, false>::initHtSensor_() {
  fixed_t __h, __t;
  return (__htSensor.readHT(__h, __t));
}

template <>
void EqHtSensor<EQ_AM2320, false>::readHTSensor_(fixed_t &humidity,
                                                 fixed_t &temperature) {
  __htSensor.readHT(humidity, temperature);
}
#endif // (EQ_HT_SENSOR_TYPE == EQ_AM2320)
