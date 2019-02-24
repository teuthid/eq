/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#include "eq_uptime.h"

unsigned long EqUptime::days_{0};
uint8_t EqUptime::hours_{0};
uint8_t EqUptime::minutes_{0};
uint8_t EqUptime::seconds_{0};
bool EqUptime::highMillis_{false};
unsigned long EqUptime::rollOver_{0};
char EqUptime::str_[11];

void EqUptime::update() {
  unsigned long __m = millis();
  // expected rollover:
  if (__m >= 3000000000UL) {
    highMillis_ = true;
  }
  // actual rollover:
  if (__m <= 100000UL && highMillis_) {
    rollOver_++;
    highMillis_ = false;
  }
  unsigned long __s = __m / 1000;
  seconds_ = __s % 60;
  minutes_ = (__s / 60) % 60;
  hours_ = (__s / (60 * 60)) % 24;
  // first portion takes care of a rollover (around 50 days)
  days_ = (rollOver_ * 50) + (__s / (60UL * 60UL * 24UL));
}

const char *EqUptime::asString() {
  uint8_t __i = 0;
  update();
  // using sprintf() results in a larger code by 2kB
  if (days_ >= 100)
    str_[__i++] = days_ / 100 + 0x30;
  if (days_ >= 10)
    str_[__i++] = days_ / 10 + 0x30;
  str_[__i++] = days_ % 10 + 0x30;
  str_[__i++] = 'd';
  str_[__i++] = ' ';
  str_[__i++] = hours_ / 10 + 0x30;
  str_[__i++] = hours_ % 10 + 0x30;
  str_[__i++] = ':';
  str_[__i++] = minutes_ / 10 + 0x30;
  str_[__i++] = minutes_ % 10 + 0x30;
  str_[__i] = 0;
  return str_;
}
