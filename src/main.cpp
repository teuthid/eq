
#ifdef EQ_DEBUG
#define _TASK_DEBUG
#endif

#include "eq_tasks.h"
#include <TaskScheduler.h>

Scheduler eqController;

void setup() {
  Serial.begin(115200);
#ifdef EQ_DEBUG
  Serial.print(F("Initializing... "));
#endif
  if (EqConfig::init()) {
    eqController.addTask(EqTask<EqTaskId::Heartbeat>::instance());
    eqController.addTask(EqTask<EqTaskId::ItSensorControl>::instance());
    eqController.addTask(EqTask<EqTaskId::HtSensorControl>::instance());
    eqController.addTask(EqTask<EqTaskId::FanControl>::instance());
    eqController.addTask(EqTask<EqTaskId::ButtonControl>::instance());
#ifdef EQ_DEBUG
    eqController.addTask(EqTask<EqTaskId::Debug>::instance());
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
