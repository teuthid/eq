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
  EqFlashString(const void *address) : address_(address) {}
  size_t length() const;
  int compare(const char *str) const;
  int compare_n(const char *str, size_t n) const;
  char *copy(char *str, size_t offset = 0) const;
  char *copy_n(char *str, size_t n, size_t offset = 0) const;

private:
  const void *address_;
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
