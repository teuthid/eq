
#include "eq_tasks.h"
#include "eq_display.h"
#include "eq_fan_pwm.h"
#include "eq_ht_sensor.h"
#include "eq_led.h"
#include "eq_light_sensor.h"

EqHeartbeat::EqHeartbeat(Scheduler *scheduler)
    : Task(TASK_SECOND, TASK_FOREVER, scheduler, false) {}

bool EqHeartbeat::Callback() {
  eqLightSensor.read();
  if (EqConfig::anyAlert())
    eqLedAlert.toggle(true); // force blinking led
  else
    eqLedAlert.setState(false);
  eqLedHeartbeat.toggle(true);
  eqDisplay.show(eqHtSensor.lastHumidity(), eqHtSensor.lastTemperature(),
                 eqHtSensor.trendHumidity(), eqHtSensor.trendTemperature(),
                 eqFanPwm.lastSpeed());
  EqConfig::decreaseOverdriveTime();
  EqConfig::decreaseBacklightTimeCounter();
  return true;
}
