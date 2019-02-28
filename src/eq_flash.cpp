/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#include "eq_flash.h"

#ifdef EQ_ARCH_AVR

//size_t EqFlashString::length() const { return strlen_P(*address_); }

char *fstring_copy(char *buffer, const void *address_short) {
  return strcpy_P(buffer, (PGM_P)pgm_read_word(address_short));
}

char *fstring_copy_n(char *buffer, const void *address_short, size_t n) {
  return strncpy_P(buffer, (PGM_P)pgm_read_word(address_short), n);
}

#else // without using flash memory

#include <string.h>

char *fstring_copy(char *buffer, const char *const *str) {
  return strcpy(buffer, *str);
}

char *fstring_copy_n(char *buffer, const char *const *str, size_t n) {
  return strncpy(buffer, *str, n);
}

#endif
