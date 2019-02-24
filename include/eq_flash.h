/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#ifndef __EQ_FLASH_H__
#define __EQ_FLASH_H__

#ifdef EQ_ARCH_AVR
#include <avr/pgmspace.h>
#define flashchar_t const PROGMEM char
#define flashstring_t const PROGMEM char *

inline char *flashstring_copy(char *buffer, const char *fstr) {
  return strcpy_P(buffer, (PGM_P)pgm_read_word(&(fstr)));
}

#else // without using flash
#endif

#endif // __EQ_FLASH_H__
