
#ifndef __EQ_TASKS_H__
#define __EQ_TASKS_H__

#define _TASK_OO_CALLBACKS
#include <TaskSchedulerDeclarations.h>

class EqHeartbeat : public Task {
public:
  EqHeartbeat(Scheduler *scheduler);
  bool Callback();
};

#endif // __EQ_TASKS_H__
