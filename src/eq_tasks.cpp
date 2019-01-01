
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
template <>
EqTask<EqTaskId::Heartbeat>::EqTask()
    : Task(TASK_SECOND, TASK_FOREVER, nullptr, false) {
  setId(static_cast<unsigned int>(EqTaskId::Heartbeat));
}

template <> bool EqTask<EqTaskId::Heartbeat>::Callback() {
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

// internal temperature control
template <>
EqTask<EqTaskId::ItSensorControl>::EqTask()
    : Task(EqConfig::itSensorInterval * TASK_SECOND, TASK_FOREVER, nullptr,
           false) {
  setId(static_cast<unsigned int>(EqTaskId::ItSensorControl));
}

template <> bool EqTask<EqTaskId::ItSensorControl>::Callback() {
  if (!eqItSensor.read())
    EqConfig::setAlert(EqAlertType::ItSensor);
  else {
    EqConfig::resetAlert(EqAlertType::ItSensor);
    if (eqItSensor.temperature() > EqConfig::itSensorMaxTemperature) {
      EqConfig::setAlert(EqAlertType::Overheating);
      EqConfig::registerOverheating();
      if (EqConfig::overheating()) {
        Scheduler::currentScheduler().disableAll();
        EqConfig::sleep();
      }
    } else
      EqConfig::resetAlert(EqAlertType::Overheating);
  }
  return true;
}

// huminidity & temperature control
template <>
EqTask<EqTaskId::HtSensorControl>::EqTask()
    : Task(EqConfig::htSensorInterval() * TASK_SECOND, TASK_FOREVER, nullptr,
           false) {
  setId(static_cast<unsigned int>(EqTaskId::HtSensorControl));
}

template <> bool EqTask<EqTaskId::HtSensorControl>::Callback() {
  if (!eqHtSensor.read())
    EqConfig::setAlert(EqAlertType::HtSensor);
  else
    EqConfig::resetAlert(EqAlertType::HtSensor);
  return true;
}

// fan control
template <>
EqTask<EqTaskId::FanControl>::EqTask()
    : Task(EqConfig::fanPwmInterval() * TASK_SECOND, TASK_FOREVER, nullptr,
           false) {
  setId(static_cast<unsigned int>(EqTaskId::FanControl));
}

template <> bool EqTask<EqTaskId::FanControl>::Callback() {
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

// button control
template <>
EqTask<EqTaskId::ButtonControl>::EqTask()
    : Task(EqConfig::buttonReadInterval * TASK_MILLISECOND, TASK_FOREVER,
           nullptr, false) {
  setId(static_cast<unsigned int>(EqTaskId::ButtonControl));
}

template <> bool EqTask<EqTaskId::ButtonControl>::Callback() {
  eqButtonBacklight.read();
  eqButtonOverdrive.read();
  return true;
}

// debugging
#ifdef EQ_DEBUG
EqDebugTask::EqDebugTask(Scheduler *scheduler)
    : Task(EqConfig::debugInterval * TASK_SECOND, TASK_FOREVER, scheduler,
           false) {}

template <>
void EqDebugTask::print_(const __FlashStringHelper *description,
                         const fixed_t &value) {
  Serial.print(description);
  Serial.print(fixed_to_float(value));
}

bool EqDebugTask::Callback() {
  print_(F("L="), eqLightSensor.intensity());
  print_(F(" H="), eqHtSensor.lastHumidity());
  print_(F(" T="), eqHtSensor.lastTemperature());
  print_(F(" F="), eqFanPwm.lastSpeed());
  print_(F(" I="), eqItSensor.lastTemperature());
  print_(F(" M="), freeMemory());
  Serial.println();
  return true;
}
#endif // EQ_DEBUG
