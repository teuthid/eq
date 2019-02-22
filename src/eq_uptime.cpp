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
