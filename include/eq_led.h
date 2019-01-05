
#ifndef __EQ_LED_H__
#define __EQ_LED_H__

#include "eq_config.h"
#include "eq_dpin.h"

template <uint8_t LedPin> class EqLed;
using EqLedAlert = EqLed<EqConfig::ledAlertPin>;
using EqLedHeartbeat = EqLed<EqConfig::ledHeartbeatPin>;

template <uint8_t LedPin> class EqLed {
  friend EqLedAlert &eqLedAlert();
  friend EqLedHeartbeat &eqLedHeartbeat();

public:
  void setState(const bool &newState);
  void test(const uint32_t &interval /* in milliseconds */,
            const uint8_t &iterations);
  void toggle(const bool &force = false);
  
  EqLed(const EqLed &) = delete;
  EqLed(EqLed &&) = delete;
  void operator=(const EqLed &) = delete;

private:
  EqLed();
  static EqLed instance_;
};

inline EqLedAlert &eqLedAlert() { return EqLedAlert::instance_; }
inline EqLedHeartbeat &eqLedHeartbeat() { return EqLedHeartbeat::instance_; }

template <uint8_t LedPin> EqLed<LedPin>::EqLed() {
  EqDPin<LedPin>::setOutputLow();
}

template <uint8_t LedPin> void EqLed<LedPin>::setState(const bool &state) {
  EqDPin<LedPin>::setOutputValue(state);
}

template <uint8_t LedPin>
void EqLed<LedPin>::test(const uint32_t &interval, const uint8_t &iterations) {
  if (iterations > 0)
    for (uint16_t __i = 0; __i < (2 * iterations); __i++) {
      EqDPin<LedPin>::setOutputValueToggle();
      delay(interval);
    }
}

template <uint8_t LedPin> void EqLed<LedPin>::toggle(const bool &force) {
  if (force || EqConfig::backlight())
    EqDPin<LedPin>::setOutputValueToggle();
  else
    EqDPin<LedPin>::setOutputValue(LOW);
}

#endif // __EQ_LED_H__
