
#include "eq_tasks.h"
#include "eq_button.h"
#include "eq_display.h"
#include "eq_ht_sensor.h"
#include "eq_led.h"
#include "eq_light_sensor.h"

// heartbeat
EqHeartbeat::EqHeartbeat(Scheduler *scheduler)
    : Task(TASK_SECOND, TASK_FOREVER, scheduler, false) {}

bool EqHeartbeat::Callback() {
  eqLightSensor.read();
  if (EqConfig::anyAlert())
    eqLedAlert.toggle(true); // force blinking led
  else
    eqLedAlert.setState(false);
  eqLedHeartbeat.toggle(true);
  eqDisplay.show();
  EqConfig::decreaseOverdriveTime();
  EqConfig::decreaseBacklightTimeCounter();
  return true;
}

// button control
EqButtonControl::EqButtonControl(Scheduler *scheduler)
    : Task(EqConfig::buttonReadInterval * TASK_MILLISECOND, TASK_FOREVER,
           scheduler, false) {}

bool EqButtonControl::Callback() {
  eqButtonBacklight.read();
  eqButtonOverdrive.read();
  return true;
}

// huminidity & temperature control
EqHtSensorControl::EqHtSensorControl(Scheduler *scheduler)
    : Task(EqConfig::htSensorInterval() * TASK_SECOND, TASK_FOREVER, scheduler,
           false) {}

bool EqHtSensorControl::Callback() {
  if (!eqHtSensor.read())
    EqConfig::setAlert(EqAlertType::HtSensor);
  else
    EqConfig::resetAlert(EqAlertType::HtSensor);

  return true;
}
