/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#include "eq_flash.h"

#ifdef EQ_ARCH_AVR

char *fstring_copy(char *buffer, const void *address_short) {
  return strcpy_P(buffer, (PGM_P)pgm_read_word(address_short));
}

char *fstring_copy_n(char *buffer, const void *address_short, size_t n) {
  return strncpy_P(buffer, (PGM_P)pgm_read_word(address_short), n);
}

#else // without using flash memory
#endif
