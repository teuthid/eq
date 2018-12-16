
#include "eq_collector.h"

template <> bool EqCollector<(uint8_t)1>::add(const fixed_t &value) {
  collector_[0] = value;
  average_ = value;
  return true;
}
