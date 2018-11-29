
#ifndef __EQ_COLLECTOR_H__
#define __EQ_COLLECTOR_H__

#define FIXED_POINTS_USE_NAMESPACE
#include <FixedPoints.h>
#include <FixedPointsCommon.h>

template <uint8_t Size> class EqCollector {
  static_assert(Size > 0, "The size of EqCollector should be greater than 0");

public:
  constexpr EqCollector() {}

  void clear(const float &value = 0.0) {
    for (uint8_t __i = 0; __i < Size; __i++)
      collector_[__i] = value;
  }
  void setAcceptableValueRange(const float &lowerBound,
                               const float &upperBound) {
    lowerBound_ = lowerBound;
    upperBound_ = upperBound;
    controlValueRange_ = true;
  }

  bool add(const float &value);

  constexpr float average() const { return static_cast<float>(average_); }
  constexpr long averageAsLong() const {
    return (FixedPoints::roundFixed(average_)).getInteger();
  }
  constexpr uint8_t deviation() const { return deviation_; } // in percents
  constexpr float last() const {
    return static_cast<float>(collector_[Size - 1]);
  }
  constexpr long lastAsLong() const {
    return (FixedPoints::roundFixed(collector_[Size - 1])).getInteger();
  }
  constexpr int8_t trend() const { return trend_; };

private:
  typedef FixedPoints::SQ15x16 FPType_;
  FPType_ collector_[Size];
  bool controlValueRange_ = false;
  FPType_ lowerBound_ = 0, upperBound_ = 0;
  FPType_ average_ = 0;
  int8_t trend_ = 0;
  uint8_t deviation_ = 0;

  constexpr bool inRange_(const FPType_ &value) const {
    return (value > upperBound_) ? false
                                 : ((value < lowerBound_) ? false : true);
  }
};

template <uint8_t Size> bool EqCollector<Size>::add(const float &value) {
  if (controlValueRange_ && !inRange_(value))
    return false;

  for (uint8_t __i = 1; __i < Size; __i++)
    collector_[__i - 1] = collector_[__i];
  collector_[Size - 1] = value;

  average_ = 0;
  trend_ = 0;
  FPType_ __x = 0;
  for (uint8_t __i = 0; __i < Size; __i++) {
    average_ += (collector_[__i] / Size);
    if (__i > 0) {
      trend_ +=
          (FixedPoints::roundFixed(__i * (collector_[__i] - collector_[0])))
              .getInteger();
      __x += FixedPoints::absFixed(collector_[__i] - collector_[__i - 1]);
    }
  }
  long __d = (average_ == 0) // FIXME
                 ? 0
                 : (FixedPoints::roundFixed(100 * __x / average_)).getInteger();
  deviation_ = min((__d / Size), 100);
  return true;
}

template <> bool EqCollector<(uint8_t)1>::add(const float &value);

#endif // __EQ_COLLECTOR_H__
