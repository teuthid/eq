/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#include "eq_uptime.h"

unsigned long EqUptime::days_{0};
uint8_t EqUptime::hours_{0};
uint8_t EqUptime::minutes_{0};
uint8_t EqUptime::seconds_{0};
bool EqUptime::highMillis_{false};
unsigned long EqUptime::rollOver_{0};
