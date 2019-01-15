/*
   eq - transcendental fan controller ;)
   Copyright (c) 2017-2018 Mariusz Przygodzki
*/

#include "eq_tasks.h"
#include "eq_button.h"
#include "eq_display.h"
#include "eq_fan_pwm.h"
#include "eq_ht_sensor.h"
#include "eq_led.h"
#include "eq_light_sensor.h"

// heartbeat
template <>
EqTaskHeartbeat::EqTask() : Task(TASK_SECOND, TASK_FOREVER, nullptr, false) {
  setId(static_cast<unsigned int>(EqTaskId::Heartbeat));
}

template <> bool EqTaskHeartbeat::Callback() {
  eqLightSensor().read();
  setWdPoint(1);
  if (EqConfig::anyAlert())
    eqLedAlert().toggle(true); // force blinking led
  else
    eqLedAlert().setState(false);
  eqLedHeartbeat().toggle(true);
  eqDisplay().show();
  setWdPoint(2);
  EqConfig::decreaseOverdriveTime();
  EqConfig::decreaseBacklightTimeCounter();
  return true;
}

// IT sensor control
template <>
EqTaskItSensorControl::EqTask()
    : Task(EqConfig::itSensorInterval * TASK_SECOND, TASK_FOREVER, nullptr,
           false) {
  setId(static_cast<unsigned int>(EqTaskId::ItSensorControl));
}

template <> bool EqTaskItSensorControl::Callback() {
  if (!eqItSensor().read())
    EqConfig::setAlert(EqAlertType::ItSensor);
  else {
    setWdPoint(1);
    EqConfig::resetAlert(EqAlertType::ItSensor);
    if (eqItSensor().temperature() > EqConfig::itSensorMaxTemperature) {
      EqConfig::setAlert(EqAlertType::Overheating);
      EqConfig::registerOverheating();
      setWdPoint(2);
      if (EqConfig::overheating()) {
        setWdPoint(3);
        Scheduler::currentScheduler().disableAll();
        EqConfig::sleep();
      }
    } else {
      setWdPoint(4);
      EqConfig::resetAlert(EqAlertType::Overheating);
    }
  }
  return true;
}

// HT sensor control
template <>
EqTaskHtSensorControl::EqTask()
    : Task(EqConfig::htSensorInterval() * TASK_SECOND, TASK_FOREVER, nullptr,
           false) {
  setId(static_cast<unsigned int>(EqTaskId::HtSensorControl));
}

template <> bool EqTaskHtSensorControl::Callback() {
  if (!eqHtSensor().read()) {
    setWdPoint(1);
    EqConfig::setAlert(EqAlertType::HtSensor);
  } else {
    setWdPoint(2);
    EqConfig::resetAlert(EqAlertType::HtSensor);
    EqConfig::setLedStatus();
  }
  return true;
}

// fan control
template <>
EqTaskFanControl::EqTask()
    : Task(EqConfig::fanPwmInterval() * TASK_SECOND, TASK_FOREVER, nullptr,
           false) {
  setId(static_cast<unsigned int>(EqTaskId::FanControl));
}

template <> bool EqTaskFanControl::Callback() {
  // setting pwm:
  if (!EqConfig::anyAlert())
    if (EqConfig::overdriveTime() > 0) {
      setWdPoint(1);
      eqFanPwm().setOverdrive();
    } else {
      setWdPoint(2);
      eqFanPwm().setDutyCycle();
    }
  else {
    setWdPoint(3);
    eqFanPwm().stop();
  }
  // reading fan speed:
  if (!eqFanPwm().readSpeed()) {
    setWdPoint(4);
    EqConfig::setAlert(EqAlertType::Fan);
  } else {
    setWdPoint(5);
    EqConfig::resetAlert(EqAlertType::Fan);
  }
  return true;
}

// buttons control
template <>
EqTaskButtonControl::EqTask()
    : Task(EqConfig::buttonReadInterval * TASK_MILLISECOND, TASK_FOREVER,
           nullptr, false) {
  setId(static_cast<unsigned int>(EqTaskId::ButtonControl));
}

template <> bool EqTaskButtonControl::Callback() {
  eqButtonBacklight().read();
  setWdPoint(1);
  eqButtonOverdrive().read();
  return true;
}

// debugging
#ifdef EQ_DEBUG
#include "MemoryFree.h"
template <>
EqTaskDebug::EqTask()
    : Task(EqConfig::debugInterval * TASK_SECOND, TASK_FOREVER, nullptr,
           false) {
  setId(static_cast<unsigned int>(EqTaskId::Debug));
}

template <> bool EqTaskDebug::Callback() {
  EqConfig::printValue(F("L="), eqLightSensor().intensity());
  EqConfig::printValue(F(" H="), fixed_to_float(eqHtSensor().lastHumidity()));
  EqConfig::printValue(F(" T="),
                       fixed_to_float(eqHtSensor().lastTemperature()));
  EqConfig::printValue(F(" F="), eqFanPwm().lastSpeed());
  EqConfig::printValue(F(" I="),
                       fixed_to_float(eqItSensor().lastTemperature()));
  EqConfig::printValue(F(" M="), freeMemory());
  Serial.println();
  return true;
}
#endif // EQ_DEBUG

// instances of EqTask
template <> EqTaskHeartbeat EqTaskHeartbeat::instance_{};
template <> EqTaskItSensorControl EqTaskItSensorControl::instance_{};
template <> EqTaskHtSensorControl EqTaskHtSensorControl::instance_{};
template <> EqTaskFanControl EqTaskFanControl::instance_{};
template <> EqTaskButtonControl EqTaskButtonControl::instance_{};
#ifdef EQ_DEBUG
template <> EqTaskDebug EqTaskDebug::instance_{};
#endif
