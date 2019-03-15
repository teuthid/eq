/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#include "eq_config.h"

test(EqFlashString) {
  EQ_FSTRING(strTest, "0123456789");
  EqFlashString __str(strTest);
  assertEqual(__str.length(), size_t(10));
  assertEqual(__str.compare("0123456789"), 0);
  assertTrue(__str.compare("01234") > 0);
  assertTrue(__str.compare("1") < 0);
  assertEqual(__str.compare_n("01234", 5), 0);
  assertNotEqual(__str.compare_n("01234", 6), 0);
  char __s[25];
  __str.copy(__s);
  assertEqual(__str.compare("0123456789"), 0);
  __str.copy(__s, 1);
  assertEqual(strcmp(__s, "123456789"), 0);
  __str.copy_n(__s, 5);
  assertEqual(strncmp(__s, "01234", 5), 0);
  __str.copy(__s);
  __str.append(__s);
  assertEqual(strcmp(__s, "01234567890123456789"), 0);
  __str.copy(__s);
  __str.append_n(__s, 5);
  assertEqual(strcmp(__s, "012345678901234"), 0);
  assertEqual(strcmp(__str(__s), "0123456789"), 0);
  for (uint8_t __i = 0; __i < 10; __i++) {
    assertEqual(__str.charAt(__i), char(__i + 48));
    assertEqual(__str[__i], char(__i + 48));
  }
  aunit::fake::FakePrint fakePrint;
  fakePrint.print(__str);
  assertTrue(strcmp("0123456789", fakePrint.getBuffer())); // ???
}

