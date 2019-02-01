/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

// implementation for AVR only

#ifdef EQ_ARCH_AVR

#include <avr/sleep.h>
#include <avr/wdt.h>

#include "eq_config.h"
#include "eq_fan_pwm.h"
#include "eq_led.h"
#include "eq_timer.h"

void EqConfig::disableWatchdog() { wdt_disable(); }

void EqConfig::enableWatchdog() {
  cli();
  wdt_reset();
  // set up WDT interrupt
  WDTCSR = (1 << WDCE) | (1 << WDE);
  // start watchdog timer with aDelay prescaller
  WDTCSR = (1 << WDIE) | (1 << WDE) | (WDTO_2S & 0x2F);
  //  WDTCSR = (1<<WDIE)|(WDTO_2S & 0x2F);  // interrupt only without reset
  sei();
}

void EqConfig::resetWatchdog() { wdt_reset(); }

void EqConfig::sleep() {
  if (EqConfig::isFanTachometerEnabled())
    EqFanPwm::stopTachometer();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  attachInterrupt(digitalPinToInterrupt(EqConfig::buttonOverdrivePin), []() {},
                  LOW);
  sei();
  eqTimer().setPwm(0);
  eqLedHeartbeat().setState(false);
  eqLedAlert().setState(true);
#if (EQ_LED_STATUS_ENABLED)
  eqLedStatus().setState(false);
#endif
  sleep_mode();
  // executed after the interrupt:
  sleep_disable();
  EqConfig::reset();
}

#endif // EQ_ARCH_AVR