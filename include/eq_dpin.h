/*
   EQ - transcendental fan controller ;)
   Copyright (c) 2017-2018 Mariusz Przygodzki
*/

#ifndef __EQ_DPIN_H__
#define __EQ_DPIN_H__

#ifdef EQ_ARCH_AVR
#include <FastGPIO.h>
#else
#include "Arduino.h"
#endif

template <uint8_t DPin> class EqDPin final {
public:
  static inline void setOutput(const bool &value) {
#ifdef EQ_ARCH_AVR
    FastGPIO::Pin<DPin>::setOutput(value);
#else
    pinMode(DPin, OUTPUT);
    digitalWrite(DPin, value ? HIGH : LOW);
#endif
  }

  static inline void setOutputLow() {
#ifdef EQ_ARCH_AVR
    FastGPIO::Pin<DPin>::setOutputLow();
#else
    pinMode(DPin, OUTPUT);
    digitalWrite(DPin, LOW);
#endif
  }

  static inline void setOutputHigh() {
#ifdef EQ_ARCH_AVR
    FastGPIO::Pin<DPin>::setOutputHigh();
#else
    pinMode(DPin, OUTPUT);
    digitalWrite(DPin, HIGH);
#endif
  }

  static inline void setOutputToogle() {
#ifdef EQ_ARCH_AVR
    FastGPIO::Pin<DPin>::setOutputToogle();
#else
    pinMode(DPin, OUTPUT);
    digitalWrite(DPin, (digitalRead(DPin) == HIGH) ? LOW : HIGH);
#endif
  }

  static inline void setOutputValue(const bool &value) {
#ifdef EQ_ARCH_AVR
    FastGPIO::Pin<DPin>::setOutputValue(value);
#else
    digitalWrite(DPin, value ? HIGH : LOW);
#endif
  }

  static inline void setOutputValueLow() {
#ifdef EQ_ARCH_AVR
    FastGPIO::Pin<DPin>::setOutputValueLow();
#else
    digitalWrite(DPin, LOW);
#endif
  }

  static inline void setOutputValueHigh() {
#ifdef EQ_ARCH_AVR
    FastGPIO::Pin<DPin>::setOutputValueHigh();
#else
    digitalWrite(DPin, HIGH);
#endif
  }

  static inline void setOutputValueToggle() {
#ifdef EQ_ARCH_AVR
    FastGPIO::Pin<DPin>::setOutputValueToggle();
#else
    digitalWrite(DPin, (digitalRead(DPin) == HIGH) ? LOW : HIGH);
#endif
  }

  static inline void setInput() {
#ifdef EQ_ARCH_AVR
    FastGPIO::Pin<DPin>::setInput();
#else
    pinMode(DPin, INPUT);
#endif
  }

  static inline void setInputPulledUp() {
#ifdef EQ_ARCH_AVR
    FastGPIO::Pin<DPin>::setInputPulledUp();
#else
    pinMode(DPin, INPUT_PULLUP);
#endif
  }

  static inline bool isInputHigh() {
#ifdef EQ_ARCH_AVR
    return FastGPIO::Pin<DPin>::isInputHigh();
#else
    return (digitalRead(DPin) == HIGH);
#endif
  }

  static inline bool isInputLow() { return !isInputHigh(); }

private:
  EqDPin() {}
  ~EqDPin() {}
};

#endif //
