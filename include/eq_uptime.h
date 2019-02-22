/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#ifndef __EQ_UPTIME_H__
#define __EQ_UPTIME_H__

#include "Arduino.h"

class EqUptime final {
public:
  static unsigned long days() { return days_; }
  static uint8_t hours() { return hours_; }
  static uint8_t minutes() { return minutes_; }
  static uint8_t seconds() { return seconds_; }
  static void update();

private:
  EqUptime() {}
  static unsigned long days_;
  static uint8_t hours_;
  static uint8_t minutes_;
  static uint8_t seconds_;
  static bool highMillis_;
  static unsigned long rollOver_;
};

#endif // __EQ_UPTIME_H__
