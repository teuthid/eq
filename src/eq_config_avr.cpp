/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

// implementation for AVR only

#ifdef EQ_ARCH_AVR

#include <avr/sleep.h>
#include <avr/wdt.h>

#include "eq_config.h"

void EqConfig::disableWatchdog() {
  if (watchdogEnabled_) {
    wdt_disable();
    watchdogEnabled_ = false;
  }
}

void EqConfig::enableWatchdog() {
  if (!watchdogEnabled_) {
    cli();
    wdt_reset();
    // set up WDT interrupt
    WDTCSR = (1 << WDCE) | (1 << WDE);
    // start watchdog timer (delay of 2s)
    WDTCSR = (1 << WDIE) | (1 << WDE) | (WDTO_2S & 0x2F);
    //  WDTCSR = (1<<WDIE)|(WDTO_2S & 0x2F);  // interrupt only without reset
    sei();
    watchdogEnabled_ = true;
  }
}

void EqConfig::resetWatchdog() {
  if (watchdogEnabled_)
    wdt_reset();
}

void EqConfig::sleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  attachInterrupt(digitalPinToInterrupt(EqConfig::buttonOverdrivePin), []() {},
                  !EqConfig::buttonOverdriveInvert);
  sei();
  sleep_mode();
  // executed after the interrupt:
  sleep_disable();
  EqConfig::reset();
}

ISR(WDT_vect) // watchdog timeout ISR
{
  EqConfig::saveWatchdogPoint();
}

#endif // EQ_ARCH_AVR
