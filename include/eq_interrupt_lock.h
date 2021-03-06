/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#ifndef __EQ_INTERRUPT_LOCK_H__
#define __EQ_INTERRUPT_LOCK_H__

#ifdef EQ_ARCH_AVR
#include <avr/interrupt.h>

class EqInterruptLock {
public:
  EqInterruptLock() {
    sreg_ = SREG;
    cli();
  }
  ~EqInterruptLock() { SREG = sreg_; }
  EqInterruptLock(const EqInterruptLock &) = delete;
  EqInterruptLock(EqInterruptLock &&) = delete;
  void operator=(const EqInterruptLock &) = delete;

private:
  uint8_t sreg_;
};

#else
// TODO: other architectures
#endif

#define EQ_INTERRUPT_LOCK EqInterruptLock __eq_interrupt_lock;

#endif // __EQ_INTERRUPT_LOCK_H__
