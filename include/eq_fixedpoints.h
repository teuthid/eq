/*
   eq - transcendental fan controller ;)
   Copyright (c) 2017-2018 Mariusz Przygodzki
*/

#ifndef __EQ_FIXEDPOINTS_H__
#define __EQ_FIXEDPOINTS_H__

#define FIXED_POINTS_USE_NAMESPACE
#include <FixedPoints.h>
#include <FixedPointsCommon.h>

using fixed_t = FixedPoints::SQ15x16;

inline constexpr fixed_t fixed_abs(const fixed_t &value) {
  return FixedPoints::absFixed(value);
}

inline constexpr fixed_t fixed_ceil(const fixed_t &value) {
  return FixedPoints::ceilFixed(value);
}

inline constexpr fixed_t fixed_floor(const fixed_t &value) {
  return FixedPoints::floorFixed(value);
}

inline constexpr fixed_t fixed_round(const fixed_t &value) {
  return FixedPoints::roundFixed(value);
}

inline constexpr bool fixed_signbit(const fixed_t &value) {
  return FixedPoints::signbitFixed(value);
}

inline constexpr fixed_t fixed_trunc(const fixed_t &value) {
  return FixedPoints::truncFixed(value);
}

inline constexpr long fixed_to_long(const fixed_t &value) {
  return static_cast<long>(FixedPoints::roundFixed(value).getInteger());
}

inline constexpr int fixed_to_int(const fixed_t &value) {
  return static_cast<int>(FixedPoints::roundFixed(value).getInteger());
}

inline constexpr float fixed_to_float(const fixed_t &value) {
  return static_cast<float>(value);
}

#endif // __EQ_FIXEDPOINTS_H__
