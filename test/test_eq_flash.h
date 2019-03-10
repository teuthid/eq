/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#include "eq_config.h"

test(EqFlashString) {
  EQ_FSTRING(strTest, "0123456789");
  EqFlashString __str(strTest);
  uint8_t __length = __str.length();
  assertEqual(__length, 10);
  assertTrue(__str.compare("0123456789") == 0);
  assertTrue(__str.compare("01234") > 0);
  assertTrue(__str.compare("1") < 0);
  assertTrue(__str.compare_n("01234", 5) == 0);
  assertTrue(__str.compare_n("01234", 6) != 0);
  char __s[25];
  __str.copy(__s);
  assertTrue(__str.compare("0123456789") == 0);
  __str.copy(__s, 1);
  assertTrue(strcmp(__s, "123456789") == 0);
  __str.copy_n(__s, 5);
  assertTrue(strncmp(__s, "01234", 5) == 0);
  __str.copy(__s);
  __str.append(__s);
  assertTrue(strcmp(__s, "01234567890123456789") == 0);
  __str.copy(__s);
  __str.append_n(__s, 5);
  assertTrue(strcmp(__s, "012345678901234") == 0);
  assertTrue(strcmp(__str(__s), "0123456789") == 0);
  for (uint8_t __i = 0; __i < 10; __i++) {
    assertTrue(__str.charAt(__i) == (char)(__i + 48));
    assertTrue(__str[__i] == (char)(__i + 48));
  }
  aunit::fake::FakePrint fakePrint;
  fakePrint.print(__str);
  assertEqual("0123456789", fakePrint.getBuffer());
}

static const PROGMEM char *const strArrayTest[] = {"1", "22", "333", "4444",
                                                   "55555"};

test(EqFlashStringArray) {
  EqFlashStringArray<sizeof(strArrayTest) / sizeof(strArrayTest[0])> __strArray(
      strArrayTest);
  assertTrue(__strArray.size() == 5);
  Serial.println(__strArray[0]);
  // assertTrue(__strArray[0].compare("1") == 0);
}
