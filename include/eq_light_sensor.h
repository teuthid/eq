
#ifndef __EQ_LIGHT_SENSOR_H__
#define __EQ_LIGHT_SENSOR_H__

#include "eq_collector.h"
#include "eq_config.h"

class EqLightSensor {
public:
  constexpr EqLightSensor() {}
  bool init();
  void read();
  constexpr uint8_t intensity() const { return intensity_; }

private:
  enum State : uint8_t { Low, Rising, High };

  uint8_t intensity_ = 0;
  State state_ = State::Low;
  EqCollector<EqConfig::lightSensorCollectorSize> collector_;
  void collect_();
  void determineState_();
};

extern EqLightSensor eqLightSensor;

#endif // __EQ_LIGHT_SENSOR_H__
