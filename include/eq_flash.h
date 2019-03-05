/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#ifndef __EQ_FLASH_H__
#define __EQ_FLASH_H__

#ifdef EQ_ARCH_AVR

#include <avr/pgmspace.h>

class EqFlashString {
public:
  EqFlashString(const void *pstr) {
    pstr_ = reinterpret_cast<const char *>(pstr);
  }
  size_t length() const { return strlen_P(pstr_); }
  int compare(const char *str) const { return -strcmp_P(str, pstr_); }
  int compare_n(const char *str, size_t n) const {
    return -strncmp_P(str, pstr_, n);
  }
  char *copy(char *str, size_t offset = 0) const {
    return strcpy_P(str, pstr_ + offset);
  }
  char *copy_n(char *str, size_t n, size_t offset = 0) const {
    return strncpy_P(str, pstr_ + offset, n);
  }
  char *append(char *str) const { return strcat_P(str, pstr_); }
  char *append_n(char *str, size_t n) const { return strncat_P(str, pstr_, n); }

  char operator[](size_t index) const {
    return static_cast<char>(pgm_read_byte(pstr_ + index));
  }

private:
  const char *pstr_;
};

// TO REMOVE:
#define fchar_t const PROGMEM char
#define fstring_t const PROGMEM char *
char *fstring_copy(char *buffer, const void *address_short);
char *fstring_copy_n(char *buffer, const void *address_short, size_t n);

#else // without using flash memory

// TODO

#endif

#endif // __EQ_FLASH_H__
