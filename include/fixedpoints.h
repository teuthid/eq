
#define FIXED_POINTS_USE_NAMESPACE
#include <FixedPoints.h>
#include <FixedPointsCommon.h>

using fixed_t = FixedPoints::SQ15x16;

inline constexpr fixed_t fixed_abs(fixed_t &value) {
  return FixedPoints::absFixed(value);
}

inline constexpr fixed_t fixed_ceil(fixed_t &value) {
  return FixedPoints::ceilFixed(value);
}

inline constexpr fixed_t fixed_floor(fixed_t &value) {
  return FixedPoints::floorFixed(value);
}

inline constexpr fixed_t fixed_round(fixed_t &value) {
  return FixedPoints::roundFixed(value);
}

inline constexpr bool fixed_signbit(fixed_t &value) {
  return FixedPoints::signbitFixed(value);
}

inline constexpr int fixed_to_int(fixed_t &value) { return value.getInteger(); }
