/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#ifndef __EQ_FLASH_H__
#define __EQ_FLASH_H__

#ifdef EQ_ARCH_AVR

#include <avr/pgmspace.h>

class EqFlashString final {
public:
  EqFlashString(const void *address) : address_(address) {}
  //size_t length() const;

private:
  const void *address_;
};

#define fchar_t const PROGMEM char
#define fstring_t const PROGMEM char *
char *fstring_copy(char *buffer, const void *address_short);
char *fstring_copy_n(char *buffer, const void *address_short, size_t n);

#else // without using flash memory

#include <stdlib.h> // for size_t
#define fchar_t const char
#define fstring_t const char *
char *fstring_copy(char *buffer, const char *const *str);
char *fstring_copy_n(char *buffer, const char *const *str, size_t n);

#endif

#endif // __EQ_FLASH_H__
