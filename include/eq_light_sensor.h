/*
   eq - transcendental fan controller ;)
   Copyright (c) 2017-2018 Mariusz Przygodzki
*/

#ifndef __EQ_LIGHT_SENSOR_H__
#define __EQ_LIGHT_SENSOR_H__

#include "eq_collector.h"
#include "eq_config.h"

class EqLightSensor {
  friend EqLightSensor &eqLightSensor();

public:
  bool init();
  void read();
  constexpr uint8_t intensity() const { return intensity_; }
  
  EqLightSensor(const EqLightSensor &) = delete;
  EqLightSensor(EqLightSensor &&) = delete;
  void operator=(const EqLightSensor &) = delete;

private:
  enum State : uint8_t { Low, Rising, High };

  constexpr EqLightSensor() {}
  void collect_();
  void determineState_();

  uint8_t intensity_ = 0;
  State state_ = State::Low;
  EqCollector<EqConfig::lightSensorCollectorSize> collector_;
  static EqLightSensor instance_;
};

inline EqLightSensor &eqLightSensor() { return EqLightSensor::instance_; }

#endif // __EQ_LIGHT_SENSOR_H__
