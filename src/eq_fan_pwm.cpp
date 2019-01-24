/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#include "eq_fan_pwm.h"
#include "eq_display.h"
#include "eq_dpin.h"
#include "eq_ht_sensor.h"
#include "eq_interrupt_lock.h"
#include "eq_timer.h"

#include <TimerOne.h>

volatile uint32_t EqFanPwm::counter_ = 0;

bool EqFanPwm::init() {
  const char *__s = EqConfig::alertAsString(EqAlertType::Fan);
#ifdef EQ_DEBUG
  Serial.print(__s);
#endif
  eqDisplay().showMessage(__s);
  eqTimer().init();
  if (EqConfig::isFanTachometerEnabled()) {
    EqDPin<EqConfig::fanTachometerPin>::setInputPulledUp();
    attachInterrupt(digitalPinToInterrupt(EqConfig::fanTachometerPin),
                    []() {
                      EqInterruptLock __lock;
                      EqFanPwm::counter_++;
                    },
                    RISING);
    EqFanPwm::counter_ = 0;
    timeCount_ = micros();
    return calibrate_();
  } else {
    delay(1000);                        // just for showing boot message
    EqConfig::increaseOverdriveTime(5); // checking fan without tachometer
    return true;
  }
}

void EqFanPwm::setDutyCycle() {
  uint8_t __dc = constrain(eqHtSensor().index(), 0, EqConfig::fanPwmMax());
  if (EqConfig::isFanPwmStepModeEnabled())
    __dc = (__dc / 10) * 10 + ((__dc % 10) > 4 ? 10 : 0);
  __dc = (__dc > 0) ? max(__dc, EqConfig::fanPwmMin()) : 0;
  if (__dc != dutyCycle_) {
    dutyCycle_ = __dc;
    eqTimer().setPwm(dutyCycle_);
  }
}

void EqFanPwm::setOverdrive() {
  dutyCycle_ = EqConfig::fanPwmOverdrive();
  eqTimer().setPwm(dutyCycle_);
}

void EqFanPwm::stop() {
  eqTimer().setPwm(0);
  dutyCycle_ = 0;
}

bool EqFanPwm::readSpeed() {
  if (EqConfig::isFanTachometerEnabled()) {
    uint32_t __c, __dt;
    {
      EqInterruptLock __lock;
      __c = EqFanPwm::counter_;
      __dt = micros() - timeCount_;
      EqFanPwm::counter_ = 0;
      timeCount_ = micros();
    }
    lastSpeed_ = 1000000 / (__dt / __c);
    if (EqConfig::isAlertOnZeroSpeed() && (dutyCycle_ > 0))
      return (lastSpeed_ > 0);
    return true;
  } else
    return true; // fan tachometer is not enabled
}

uint8_t EqFanPwm::lastSpeed() const {
  if (EqConfig::isFanTachometerEnabled()) {
    uint8_t __s = map(lastSpeed_, 0, maxSpeed_, 0, 100);
    return constrain(__s, 0, 100);
  } else
    return dutyCycle_;
}

bool EqFanPwm::calibrate_() {
  dutyCycle_ = 0xFF;
  eqTimer().setPwm(100); // max fan speed
  maxSpeed_ = 0;
  for (uint8_t __i = 0; __i < 10; __i++) {
    eqDisplay().showCalibrating((__i + 1) * 10);
    delay(1000);
    readSpeed();
    maxSpeed_ = max(lastSpeed_, maxSpeed_);
  }
  if (maxSpeed_ == 0) {
    EqConfig::setAlert(EqAlertType::Fan);
    return false;
  }
  return true;
}

// instance of EqFanPwm
EqFanPwm EqFanPwm::instance_{};
