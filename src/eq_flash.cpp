/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#include "eq_flash.h"

#ifdef EQ_ARCH_AVR

size_t EqFlashString::length() const {
  return strlen_P(reinterpret_cast<const char *>(address_));
}

int EqFlashString::compare(const char *str) const {
  return -strcmp_P(str, reinterpret_cast<const char *>(address_));
}

int EqFlashString::compare_n(const char *str, size_t n) const {
  return -strncmp_P(str, reinterpret_cast<const char *>(address_), n);
}

char *EqFlashString::copy(char *str, size_t offset) const {
  return strcpy_P(str, reinterpret_cast<const char *>(address_) + offset);
}

char *EqFlashString::copy_n(char *str, size_t n, size_t offset) const {
  return strncpy_P(str, reinterpret_cast<const char *>(address_) + offset, n);
}

// TO REMOVE:
char *fstring_copy(char *buffer, const void *address_short) {
  return strcpy_P(buffer, (PGM_P)pgm_read_word(address_short));
}

char *fstring_copy_n(char *buffer, const void *address_short, size_t n) {
  return strncpy_P(buffer, (PGM_P)pgm_read_word(address_short), n);
}

#else // without using flash memory

// TODO

#endif
