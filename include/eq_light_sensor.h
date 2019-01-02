
#ifndef __EQ_LIGHT_SENSOR_H__
#define __EQ_LIGHT_SENSOR_H__

#include "eq_collector.h"
#include "eq_config.h"

class EqLightSensor {
public:
  static EqLightSensor &instance() {
    static EqLightSensor instance;
    return instance;
  }
  bool init();
  void read();
  uint8_t intensity() const { return intensity_; }
  EqLightSensor(const EqLightSensor &) = delete;
  EqLightSensor(EqLightSensor &&) = delete;
  void operator=(const EqLightSensor &) = delete;

private:
  enum State : uint8_t { Low, Rising, High };

  EqLightSensor() {}

  uint8_t intensity_ = 0;
  State state_ = State::Low;
  EqCollector<EqConfig::lightSensorCollectorSize> collector_;
  void collect_();
  void determineState_();
};

#endif // __EQ_LIGHT_SENSOR_H__
