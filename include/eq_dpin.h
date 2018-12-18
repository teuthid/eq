
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
    digitalWrite(DPin, value ? HIGH : LOW);
#endif
  }

private:
  EqDPin() {}
  ~EqDPin() {}
};

#endif //
