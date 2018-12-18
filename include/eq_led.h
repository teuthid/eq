
#ifndef __EQ_LED_H__
#define __EQ_LED_H__

#include "eq_config.h"
#include "eq_dpin.h"

template <uint8_t LedPin> class EqLed {
public:
  EqLed();
  void setState(const bool &newState);
  void test(const uint32_t &interval /* in milliseconds */,
            const uint8_t &iterations);
  void toggle(const bool &force = false);
};

template <uint8_t LedPin> EqLed<LedPin>::EqLed() {
  FastGPIO::Pin<LedPin>::setOutputLow();
}

template <uint8_t LedPin> void EqLed<LedPin>::setState(const bool &state) {
  FastGPIO::Pin<LedPin>::setOutputValue(state ? HIGH : LOW);
}

template <uint8_t LedPin>
void EqLed<LedPin>::test(const uint32_t &interval, const uint8_t &iterations) {
  if (iterations > 0)
    for (uint16_t __i = 0; __i < (2 * iterations); __i++) {
      FastGPIO::Pin<LedPin>::setOutputValueToggle();
      delay(interval);
    }
}

template <uint8_t LedPin> void EqLed<LedPin>::toggle(const bool &force) {
  if (force || EqConfig::backlight())
    FastGPIO::Pin<LedPin>::setOutputValueToggle();
  else
    FastGPIO::Pin<LedPin>::setOutputValue(LOW);
}

extern EqLed<EqConfig::ledAlertPin> eqLedAlert;
extern EqLed<EqConfig::ledHeartbeatPin> eqLedHeartbeat;

#endif // __EQ_LED_H__
