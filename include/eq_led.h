/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#ifndef __EQ_LED_H__
#define __EQ_LED_H__

#include "eq_config.h"

template <uint8_t LedPin, bool Invert> class EqLed;
using EqLedAlert = EqLed<EqConfig::ledAlertPin, EqConfig::ledAlertInvert>;
using EqLedHeartbeat =
    EqLed<EqConfig::ledHeartbeatPin, EqConfig::ledHeartbeatInvert>;
using EqLedStatus = EqLed<EqConfig::ledStatusPin, EqConfig::ledStatusInvert>;

template <uint8_t LedPin, bool Invert> class EqLed {
  friend EqLedAlert &eqLedAlert();
  friend EqLedHeartbeat &eqLedHeartbeat();
  friend EqLedStatus &eqLedStatus();

public:
  void setState(bool newState) const;
  void test() const;
  void toggle(bool forcing = false) const;

  EqLed(const EqLed &) = delete;
  EqLed(EqLed &&) = delete;
  void operator=(const EqLed &) = delete;

private:
  EqLed();
  static EqLed instance_;
  static constexpr uint16_t testInterval_ = 200; // in milliseconds
  static constexpr uint8_t testIterations_ = 3;
};

inline EqLedAlert &eqLedAlert() { return EqLedAlert::instance_; }
inline EqLedHeartbeat &eqLedHeartbeat() { return EqLedHeartbeat::instance_; }
inline EqLedStatus &eqLedStatus() { return EqLedStatus::instance_; }

template <uint8_t LedPin, bool Invert> EqLed<LedPin, Invert>::EqLed() {
  pinMode(LedPin, OUTPUT);
  digitalWrite(LedPin, Invert ? HIGH : LOW);
}

template <uint8_t LedPin, bool Invert>
void EqLed<LedPin, Invert>::setState(bool state) const {
  digitalWrite(LedPin, Invert ? !state : state);
}

template <uint8_t LedPin, bool Invert>
void EqLed<LedPin, Invert>::test() const {
  for (uint16_t __i = 0; __i < (2 * testIterations_); __i++) {
    digitalWrite(LedPin, (digitalRead(LedPin) == HIGH) ? LOW : HIGH);
    delay(testInterval_);
  }
}

template <uint8_t LedPin, bool Invert>
void EqLed<LedPin, Invert>::toggle(bool forcing) const {
  if (forcing || EqConfig::backlight())
    digitalWrite(LedPin, (digitalRead(LedPin) == HIGH) ? LOW : HIGH);
  else // led off
    digitalWrite(LedPin, Invert ? HIGH : LOW);
}

#endif // __EQ_LED_H__
