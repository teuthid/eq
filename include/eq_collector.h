/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#ifndef __EQ_COLLECTOR_H__
#define __EQ_COLLECTOR_H__

#include "eq_fixedpoints.h"

template <uint8_t Size> class EqCollector {
  static_assert(Size > 0, "The size of EqCollector should be greater than 0");

public:
  constexpr EqCollector() {}

  void clear(const fixed_t &value = 0);
  void setAcceptableValueRange(const fixed_t &lowerBound,
                               const fixed_t &upperBound);
  bool add(const fixed_t &value);

  constexpr fixed_t average() const { return average_; }
  constexpr long averageAsLong() const { return fixed_to_long(average_); }
  constexpr uint8_t deviation() const { return deviation_; } // in percents
  constexpr fixed_t last() const { return collector_[Size - 1]; }
  constexpr long lastAsLong() const {
    return fixed_to_long(collector_[Size - 1]);
  }
  constexpr int8_t trend() const { return trend_; };

private:
  fixed_t collector_[Size];
  bool controlValueRange_ = false;
  fixed_t lowerBound_ = 0, upperBound_ = 0;
  fixed_t average_ = 0;
  int8_t trend_ = 0;
  uint8_t deviation_ = 0;

  constexpr bool inRange_(const fixed_t &value) const {
    return (value > upperBound_) ? false
                                 : ((value < lowerBound_) ? false : true);
  }
};

template <uint8_t Size> void EqCollector<Size>::clear(const fixed_t &value) {
  for (uint8_t __i = 0; __i < Size; __i++)
    collector_[__i] = value;
}

template <uint8_t Size>
void EqCollector<Size>::setAcceptableValueRange(const fixed_t &lowerBound,
                                                const fixed_t &upperBound) {
  lowerBound_ = lowerBound;
  upperBound_ = upperBound;
  controlValueRange_ = true;
}

template <uint8_t Size> bool EqCollector<Size>::add(const fixed_t &value) {
  if (controlValueRange_ && !inRange_(value))
    return false;

  for (uint8_t __i = 1; __i < Size; __i++)
    collector_[__i - 1] = collector_[__i];
  collector_[Size - 1] = value;

  average_ = 0;
  trend_ = 0;
  fixed_t __x = 0;
  for (uint8_t __i = 0; __i < Size; __i++) {
    average_ += (collector_[__i] / Size);
    if (__i > 0) {
      trend_ += fixed_to_int(__i * (collector_[__i] - collector_[0]));
      __x += fixed_abs(collector_[__i] - collector_[__i - 1]);
    }
  }
  long __d = (average_ == 0) // FIXME
                 ? 0
                 : fixed_to_long(100 * __x / average_);
  deviation_ = min((__d / Size), 100);
  return true;
}

template <> bool EqCollector<(uint8_t)1>::add(const fixed_t &value);

#endif // __EQ_COLLECTOR_H__
