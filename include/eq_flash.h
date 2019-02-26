/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#ifndef __EQ_FLASH_H__
#define __EQ_FLASH_H__

#ifdef EQ_ARCH_AVR
#include <avr/pgmspace.h>
#define fchar_t const PROGMEM char
#define fstring_t const PROGMEM char *

#else // without using flash memory

#endif

char *fstring_copy(char *buffer, const void *address_short);
char *fstring_copy_n(char *buffer, const void *address_short, size_t n);

#endif // __EQ_FLASH_H__
