/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#include "eq_timer.h"

#ifdef EQ_ARCH_AVR
#include <TimerOne.h>

void EqTimer::init() {
  Timer1.initialize(EqConfig::fanPwmCycle);
  Timer1.pwm(EqConfig::fanPwmPin, 0);
}

void EqTimer::setPwm(const uint8_t &duty) {
  Timer1.setPwmDuty(EqConfig::fanPwmPin, map(min(duty, 100), 0, 100, 0, 1023));
}

#else
// TODO: other architectures
#endif

// instance of EqTimer
EqTimer EqTimer::instance_{};
