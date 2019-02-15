/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#include "eq_collector.h"

template <> bool EqCollector<(uint8_t)1>::add(fixed_t value) {
  collector_[0] = value;
  average_ = value;
  return true;
}
