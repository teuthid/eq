/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#include "eq_pwm_timer.h"

#ifdef EQ_ARCH_AVR
#include <TimerOne.h>

void EqPwmTimer::init() {
  Timer1.initialize(EqConfig::fanPwmCycle * 1000UL);
  Timer1.pwm(EqConfig::fanPwmPin, 0);
}

void EqPwmTimer::setDutyCycle(uint8_t duty) {
  Timer1.setPwmDuty(EqConfig::fanPwmPin, map(min(duty, 100), 0, 100, 0, 1023));
}

void EqPwmTimer::attachCallback(callback_t callback) {
  Timer1.attachInterrupt(callback);
}

void EqPwmTimer::detachCallback() { Timer1.detachInterrupt(); }

// instance of EqTimer
EqPwmTimer EqPwmTimer::instance_{};

#endif // EQ_ARCH_AVR
