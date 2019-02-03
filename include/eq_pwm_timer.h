/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#ifndef __EQ_PWM_TIMER_H__
#define __EQ_PWM_TIMER_H__

#include "eq_config.h"

class EqPwmTimer {
  friend EqPwmTimer &eqPwmTimer();

public:
  void init();
  void setDutyCycle(const uint8_t &duty); // 0% .. 100%

  EqPwmTimer(const EqPwmTimer &) = delete;
  EqPwmTimer(EqPwmTimer &&) = delete;
  void operator=(const EqPwmTimer &) = delete;

private:
  constexpr EqPwmTimer() {}
  static EqPwmTimer instance_;
};

inline EqPwmTimer &eqPwmTimer() { return EqPwmTimer::instance_; }

#endif // __EQ_PWM_TIMER_H__
