
#include "eq_tasks.h"
#include "eq_button.h"
#include "eq_display.h"
#include "eq_fan_pwm.h"
#include "eq_ht_sensor.h"
#include "eq_led.h"
#include "eq_light_sensor.h"

#ifdef EQ_DEBUG
#include "MemoryFree.h"
#endif // EQ_DEBUG

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

// internal temperature control
EqItSensorControl::EqItSensorControl(Scheduler *scheduler)
    : Task(EqConfig::itSensorInterval * TASK_SECOND, TASK_FOREVER, scheduler,
           false),
      runner_(scheduler) {}

bool EqItSensorControl::Callback() {
  if (!eqItSensor.read())
    EqConfig::setAlert(EqAlertType::ItSensor);
  else {
    EqConfig::resetAlert(EqAlertType::ItSensor);
    if (eqItSensor.temperature() > EqConfig::itSensorMaxTemperature) {
      EqConfig::setAlert(EqAlertType::Overheating);
      EqConfig::registerOverheating();
      if (EqConfig::overheating()) {
        runner_->disableAll();
        EqConfig::sleep();
      }
    } else
      EqConfig::resetAlert(EqAlertType::Overheating);
  }
  return true;
}

// fan control
EqFanControl::EqFanControl(Scheduler *scheduler)
    : Task(EqConfig::fanPwmInterval() * TASK_SECOND, TASK_FOREVER, scheduler,
           false) {}

bool EqFanControl::Callback() {
  // setting pwm:
  if (!EqConfig::anyAlert())
    if (EqConfig::overdriveTime() > 0)
      eqFanPwm.setOverdrive();
    else
      eqFanPwm.setDutyCycle();
  else {
    eqFanPwm.stop();
  }
  // reading fan speed:
  if (!eqFanPwm.readSpeed())
    EqConfig::setAlert(EqAlertType::Fan);
  else
    EqConfig::resetAlert(EqAlertType::Fan);

  return true;
}

// debugging
#ifdef EQ_DEBUG
EqDebugTask::EqDebugTask(Scheduler *scheduler)
    : Task(EqConfig::debugInterval * TASK_SECOND, TASK_FOREVER, scheduler,
           false) {}

bool EqDebugTask::Callback() {
  print_(F(" L="), eqLightSensor.intensity());
  print_(F(" H="), eqHtSensor.lastHumidity());
  print_(F(" T="), eqHtSensor.lastTemperature());
  print_(F(" F="), eqFanPwm.lastSpeed());
  print_(F(" I="), eqItSensor.lastTemperature());
  print_(F(" M="), freeMemory());
  Serial.println();
  return true;
}
#endif // EQ_DEBUG
