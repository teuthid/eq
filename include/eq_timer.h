/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#ifndef __EQ_TIMER_H__
#define __EQ_TIMER_H__

#include "eq_config.h"

class EqTimer {
  friend EqTimer &eqTimer();

public:
  void init();
  void setPwm(const uint8_t &duty); // 0% .. 100%

  EqTimer(const EqTimer &) = delete;
  EqTimer(EqTimer &&) = delete;
  void operator=(const EqTimer &) = delete;

private:
  constexpr EqTimer() {}
  static EqTimer instance_;
};

inline EqTimer &eqTimer() { return EqTimer::instance_; }

#endif // __EQ_TIMER_H__
