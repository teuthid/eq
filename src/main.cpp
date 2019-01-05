
#include "eq_tasks.h"
#include <TaskScheduler.h>

Scheduler eqController;

void setup() {
  Serial.begin(115200);
#ifdef EQ_DEBUG
  Serial.print(F("Initializing... "));
#endif
  if (EqConfig::init()) {
    eqController.addTask(eqTaskHeartbeat());
    eqController.addTask(eqTaskItSensorControl());
    eqController.addTask(eqTaskHtSensorControl());
    eqController.addTask(eqTaskFanControl());
    eqController.addTask(eqTaskButtonControl());
#ifdef EQ_DEBUG
    eqController.addTask(eqTaskDebug());
#endif
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
