/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#include "eq_tasks.h"
#include "eq_button.h"
#include "eq_display.h"
#include "eq_fan_pwm.h"
#include "eq_ht_sensor.h"
#include "eq_led.h"
#include "eq_light_sensor.h"
#include "eq_pwm_timer.h"

// heartbeat
template <>
EqTaskHeartbeat::EqTask() : Task(TASK_SECOND, TASK_FOREVER, nullptr, false) {
  setId(static_cast<unsigned int>(EqTaskId::Heartbeat));
}

template <> bool EqTaskHeartbeat::Callback() {
  EqConfig::resetWatchdog();
  eqLightSensor().read();
  setWatchdogPoint(1);
  if (EqConfig::anyAlert()) {
    eqLedAlert().toggle(true); // force blinking ledAlert
    if (EqConfig::ledStatusEnabled)
      eqLedStatus().setState(false);
  } else { // no alerts
    eqLedAlert().setState(false);
    if (EqConfig::ledStatusEnabled) {
      if (EqConfig::overdriveTime() > 0)
        eqLedStatus().toggle(true); // force blinking ledStatus
      else
        EqConfig::setLedStatus();
    }
  }
  eqLedHeartbeat().toggle(true);
  eqDisplay().show();
  setWatchdogPoint(2);
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
  EqConfig::resetWatchdog();
  if (!eqItSensor().read()) {
    EqConfig::setAlert(EqAlertType::ItSensor);
    return false;
  } else { // correct reading of the sensor
    setWatchdogPoint(1);
    EqConfig::resetAlert(EqAlertType::ItSensor);
    if (eqItSensor().temperature() > EqConfig::itSensorMaxTemperature) {
      // overheating:
      EqConfig::setAlert(EqAlertType::Overheating);
      EqConfig::registerOverheating();
      setWatchdogPoint(2);
      if (EqConfig::overheating()) { // maxCountOverheating reached
        setWatchdogPoint(3);
        EqConfig::disableAllTasks();
        if (EqConfig::isFanTachometerEnabled())
          EqFanPwm::stopTachometer();
        eqPwmTimer().setDutyCycle(0);
        eqPwmTimer().detachCallback();
        eqLedHeartbeat().setState(false);
        eqLedAlert().setState(true);
        if (EqConfig::ledStatusEnabled)
          eqLedStatus().setState(false);
        EqConfig::disableWatchdog();
        EqConfig::sleep(); // wait for pressing override button
      }
    } else { // no overheating
      setWatchdogPoint(4);
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
  EqConfig::resetWatchdog();
  if (!eqHtSensor().read()) {
    setWatchdogPoint(1);
    EqConfig::setAlert(EqAlertType::HtSensor);
    return false;
  } else { // no alerts
    setWatchdogPoint(2);
    EqConfig::resetAlert(EqAlertType::HtSensor);
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
  EqConfig::resetWatchdog();
  // setting pwm:
  if (!EqConfig::anyAlert())
    if (EqConfig::overdriveTime() > 0) {
      setWatchdogPoint(1);
      eqFanPwm().setOverdrive();
    } else { // no overdrive
      setWatchdogPoint(2);
      eqFanPwm().setDutyCycle();
    }
  else { // any alert detected
    setWatchdogPoint(3);
    eqFanPwm().stop();
    return false;
  }
  // reading fan speed:
  if (!eqFanPwm().readSpeed()) {
    setWatchdogPoint(4);
    EqConfig::setAlert(EqAlertType::Fan);
    return false;
  } else { // fan is working properly
    setWatchdogPoint(5);
    EqConfig::resetAlert(EqAlertType::Fan);
  }
  return true;
}

// blowing control
template <>
EqTaskBlowingControl::EqTask()
    : Task(EqConfig::blowingInterval() * TASK_HOUR, TASK_FOREVER, nullptr,
           false) {
  setId(static_cast<unsigned int>(EqTaskId::BlowingControl));
}

template <> bool EqTaskBlowingControl::Callback() {
  EqConfig::resetWatchdog();
  if (!EqConfig::isBlowingEnabled())
    return false;
  if (isFirstIteration())
    return true;
  if (EqConfig::overdriveTime() == 0) {
    // only if override mode is not active
    EqConfig::increaseOverdriveTime(EqConfig::blowingTime(), false);
    setWatchdogPoint(1);
  }
  return true;
}

// debugging
#if defined(EQ_DEBUG)
#include "MemoryInfo.h"
template <>
EqTaskDebug::EqTask()
    : Task(EqConfig::debugInterval * TASK_SECOND, TASK_FOREVER, nullptr,
           false) {
  setId(static_cast<unsigned int>(EqTaskId::Debug));
}

template <> bool EqTaskDebug::Callback() {
  EqConfig::resetWatchdog();
  if (EqConfig::anyAlert()) {
    Serial.print(F("ALERT: "));
    Serial.println(EqConfig::alertAsString());
  } else { // no alerts
    EqConfig::printValue(F("L="), eqLightSensor().intensity());
    EqConfig::printValue(F(" H="), fixed_to_float(eqHtSensor().lastHumidity()));
    EqConfig::printValue(F(" T="),
                         fixed_to_float(eqHtSensor().lastTemperature()));
    EqConfig::printValue(F(" F="), eqFanPwm().lastSpeed());
    EqConfig::printValue(F(" I="),
                         fixed_to_float(eqItSensor().lastTemperature()));
    EqConfig::printValue(F(" M="), getTotalAvailableMemory());
    Serial.println();
  }
  return true;
}
#endif // EQ_DEBUG

// instances of EqTask
template <> EqTaskHeartbeat EqTaskHeartbeat::instance_{};
template <> EqTaskItSensorControl EqTaskItSensorControl::instance_{};
template <> EqTaskHtSensorControl EqTaskHtSensorControl::instance_{};
template <> EqTaskFanControl EqTaskFanControl::instance_{};
template <> EqTaskBlowingControl EqTaskBlowingControl::instance_{};
#if defined(EQ_DEBUG)
template <> EqTaskDebug EqTaskDebug::instance_{};
#endif
