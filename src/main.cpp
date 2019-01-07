
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
  } else {
    EqConfig::showAlert(EqConfig::alert());
    abort();
  }
}

void loop() { eqController.execute(); }
