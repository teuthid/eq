
#ifndef __EQ_FAN_PWM_H__
#define __EQ_FAN_PWM_H__

#include "eq_config.h"

class EqFanPwm {
  friend EqFanPwm &eqFanPwm();

public:
  bool init();
  void setDutyCycle();
  constexpr uint8_t dutyCycle() const { return dutyCycle_; }
  void setOverdrive();
  void stop();
  bool readSpeed();
  uint8_t lastSpeed() const; // in percents!

private:
  uint8_t dutyCycle_ = 0;
  uint32_t lastSpeed_ = 0;
  uint32_t maxSpeed_ = 0;
  uint32_t timeCount_ = 0;
  static volatile uint32_t counter_;
  static EqFanPwm instance_;
  
  constexpr EqFanPwm() {}
  bool calibrate_();
};

inline EqFanPwm &eqFanPwm() { return EqFanPwm::instance_; }

#endif // __EQ_FAN_PWM_H__
