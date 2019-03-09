/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#ifndef __EQ_FLASH_H__
#define __EQ_FLASH_H__

#include "Arduino.h"

class EqFlashString : public Printable {
public:
  constexpr EqFlashString(const char *pstr) : pstr_(pstr) {}
  size_t length() const;
  int compare(const char *str) const;
  int compare_n(const char *str, size_t n) const;
  char *copy(char *str, size_t offset = 0) const;
  char *copy_n(char *str, size_t n, size_t offset = 0) const;
  char *append(char *str) const;
  char *append_n(char *str, size_t n) const;
  char charAt(size_t index) const;
  char operator[](size_t index) const { return charAt(index); }
  char *operator()(char *str) const { return copy(str); }
  size_t printTo(Print &p) const; // virtual method from Printable class

private:
  const char *pstr_;
};

template <size_t Size> class EqFlashStringArray {
  static_assert(Size > 0,
                "The size of EqFlashStringArray should be greater than 0");

public:
  constexpr EqFlashStringArray(const char **pstrArray)
      : pstrArray_(pstrArray) {}
  constexpr size_t size() { return Size; }

private:
  const char **pstrArray_;
};

#if defined(EQ_ARCH_AVR)
#include <avr/pgmspace.h>

// EqFlashString
inline size_t EqFlashString::length() const { return strlen_P(pstr_); }
inline int EqFlashString::compare(const char *str) const {
  return -strcmp_P(str, pstr_);
}
inline int EqFlashString::compare_n(const char *str, size_t n) const {
  return -strncmp_P(str, pstr_, n);
}
inline char *EqFlashString::copy(char *str, size_t offset) const {
  return strcpy_P(str, pstr_ + offset);
}
inline char *EqFlashString::copy_n(char *str, size_t n, size_t offset) const {
  return strncpy_P(str, pstr_ + offset, n);
}
inline char *EqFlashString::append(char *str) const {
  return strcat_P(str, pstr_);
}
inline char *EqFlashString::append_n(char *str, size_t n) const {
  return strncat_P(str, pstr_, n);
}
inline char EqFlashString::charAt(size_t index) const {
  return static_cast<char>(pgm_read_byte(pstr_ + index));
}
inline size_t EqFlashString::printTo(Print &p) const {
  return p.print((__FlashStringHelper *)pstr_);
}

#define EQ_FSTRING(name, value)                                                \
  static const char __flash_##name[] PROGMEM = value;                          \
  EqFlashString name(__flash_##name);

#else // without using flash memory

// EqFlashString
inline size_t EqFlashString::length() const { return strlen(pstr_); }
inline int EqFlashString::compare(const char *str) const {
  return -strcmp(str, pstr_);
}
inline int EqFlashString::compare_n(const char *str, size_t n) const {
  return -strncmp(str, pstr_, n);
}
inline char *EqFlashString::copy(char *str, size_t offset) const {
  return strcpy(str, pstr_ + offset);
}
inline char *EqFlashString::copy_n(char *str, size_t n, size_t offset) const {
  return strncpy(str, pstr_ + offset, n);
}
inline char *EqFlashString::append(char *str) const {
  return strcat(str, pstr_);
}
inline char *EqFlashString::append_n(char *str, size_t n) const {
  return strncat(str, pstr_, n);
}
inline char EqFlashString::charAt(size_t index) const { return pstr_[index]; }
inline size_t EqFlashString::printTo(Print &p) const { return p.print(pstr_); }

#define EQ_FSTRING(name, value)                                                \
  static const char __flash_##name[] = value;                                  \
  EqFlashString name(__flash_##name);

#endif // architectures

#endif // __EQ_FLASH_H__
