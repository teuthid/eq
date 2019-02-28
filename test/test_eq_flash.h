/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#include "eq_config.h"

const PROGMEM char strTest[] = "0123456789";

test(EqFlashString) { 
    EqFlashString __str(&strTest); 
}
