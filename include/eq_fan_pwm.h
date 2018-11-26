
#ifndef __EQ_FAN_PWM_H__
#define __EQ_FAN_PWM_H__

#include "eq_config.h"

class EqFanPwm {
public:
  constexpr EqFanPwm() {}
  void init();
  void setDutyCycle();
  constexpr uint8_t dutyCycle() const { return dutyCycle_; }
  void setOverdrive();
  void stop();
  bool readSpeed();
  uint8_t lastSpeed() const; // in percents!
  bool calibrateTachometer();

private:
  uint8_t dutyCycle_ = 0;
  uint32_t lastSpeed_ = 0;
  uint32_t maxSpeed_ = 0;
  uint32_t timeCount_ = 0;
  static volatile uint32_t counter_;
};

extern EqFanPwm eqFanPwm;

#endif // __EQ_FAN_PWM_H__
