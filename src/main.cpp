
#ifdef EQ_DEBUG
#define _TASK_DEBUG
#endif

#include "eq_tasks.h"
#include <TaskScheduler.h>

Scheduler eqController;
EqButtonControl taskButtonControl(&eqController);
EqFanControl taskFanControl(&eqController);
#ifdef EQ_DEBUG
EqDebugTask taskDebug(&eqController);
#endif

void setup() {
  Serial.begin(115200);
#ifdef EQ_DEBUG
  Serial.print(F("Initializing... "));
#endif
  if (EqConfig::init()) {
    eqController.addTask(EqTask<EqTaskId::Heartbeat>::instance());
    eqController.addTask(EqTask<EqTaskId::ItSensorControl>::instance());
    eqController.addTask(EqTask<EqTaskId::HtSensorControl>::instance());
    // taskButtonControl.enable();
    // taskFanControl.enable();
#ifdef EQ_DEBUG
    // taskDebug.enable();
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
