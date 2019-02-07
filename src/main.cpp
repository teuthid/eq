/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#include "eq_eeprom.h"
#include "eq_pwm_timer.h"
#include "eq_tasks.h"

#include <TaskScheduler.h>

Scheduler eqController;
Scheduler eqHPController; // high priority scheduler

void setup() {
  Serial.begin(115200);
#ifdef EQ_DEBUG
  Serial.print(F("Initializing... "));
#endif
  if (EqConfig::init()) {
    eqHPController.addTask(eqTaskItSensorControl());
    eqController.addTask(eqTaskHeartbeat());
    eqController.addTask(eqTaskHtSensorControl());
    eqController.addTask(eqTaskFanControl());
    eqController.addTask(eqTaskButtonControl());
    eqController.addTask(eqTaskBlowingControl());
#ifdef EQ_DEBUG
    eqController.addTask(eqTaskDebug());
#endif
    eqController.setHighPriorityScheduler(&eqHPController);
    eqController.enableAll();
#ifdef EQ_DEBUG
    Serial.println();
    EqConfig::show();
    Serial.println(F("Running..."));
#endif
    eqController.startNow();
    eqPwmTimer().attachCallback([]() { eqTaskButtonControl().restart(); });
    eqTaskBlowingControl().delay();
  } else {
    EqConfig::showAlert(EqConfig::alert());
    abort();
  }
}

void loop() { eqController.execute(); }

void EqConfig::disableAllTasks() {
  // disable all tasks in both sschedulers
  eqController.disableAll();
}

void EqConfig::saveWatchdogPoint() {
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
