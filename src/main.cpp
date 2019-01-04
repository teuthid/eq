
#include "eq_tasks.h"
#include <TaskScheduler.h>

Scheduler eqController;

void setup() {
  Serial.begin(115200);
#ifdef EQ_DEBUG
  Serial.print(F("Initializing... "));
#endif
  if (EqConfig::init()) {
    eqController.addTask(EqTaskHeartbeat::instance());
    eqController.addTask(EqTaskItSensorControl::instance());
    eqController.addTask(EqTaskHtSensorControl::instance());
    eqController.addTask(EqTaskFanControl::instance());
    eqController.addTask(EqTaskButtonControl::instance());
#ifdef EQ_DEBUG
    eqController.addTask(EqTaskDebug::instance());
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
