/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#include "eq_eeprom.h"
#include "eq_pwm_timer.h"
#include "eq_tasks.h"

#include <TaskScheduler.h>

#if defined(EQ_UNIT_TEST)
#include <AUnitVerbose.h>

#include "eq_display.h"
#include "test_eq.h"
#endif

Scheduler eqController;
Scheduler eqHPController; // high priority scheduler

void setup() {
  Serial.begin(115200);
#if !defined(EQ_UNIT_TEST)
#if defined(EQ_DEBUG)
  Serial.print(F("Initializing... "));
#endif
  if (EqConfig::init()) {
    eqHPController.addTask(eqTaskItSensorControl());
    eqController.addTask(eqTaskHeartbeat());
    eqController.addTask(eqTaskHtSensorControl());
    eqController.addTask(eqTaskFanControl());
    eqController.addTask(eqTaskBlowingControl());
#if defined(EQ_DEBUG)
    eqController.addTask(eqTaskDebug());
#endif
    eqController.setHighPriorityScheduler(&eqHPController);
    eqController.enableAll();
#if defined(EQ_DEBUG)
    Serial.println();
    EqConfig::showSettings();
    Serial.println(F("Running..."));
#endif
    eqController.startNow();
  } else {
    EqConfig::showAlert();
    abort();
  }
#else // defined(EQ_UNIT_TEST)
  Serial.println(F("Testing..."));
  EqConfig::init();
  eqDisplay().off();
#endif
}

void loop() {
#if !defined(EQ_UNIT_TEST)
  eqController.execute();
#else
  aunit::TestRunner::run();
#endif
}

void EqConfig::disableAllTasks() {
  // disable all tasks in both schedulers
  eqController.disableAll();
}

void EqConfig::saveWatchdogPoint() {
  if (saveWatchdogPoint_) {
    Task &__t = eqController.currentTask();
#ifdef EQ_DEBUG
    uint8_t __cp = __t.getControlPoint();
    if (__cp == 0) // control point is not set
      __cp = __t.getId();
    EqEeprom::writeValue<uint8_t>(EqEeprom::LastWatchdogPoint, __cp);
#else
    EqEeprom::writeValue<uint8_t>(EqEeprom::LastWatchdogPoint,
                                  static_cast<uint8_t>(__t.getId()));
#endif
  }
}
