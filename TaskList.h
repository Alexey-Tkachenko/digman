#ifndef _TASK_LIST_
#define _TASK_LIST_

#include "Scheduler.h"


template<class T, TaskPriority prty = TaskPriority::Normal, int instance = 0>
TaskPriorityPair CreateTask()
{
	static byte task[sizeof(T)];
	return TaskPriorityPair{ *new(task)T(), prty };
}

void FillTaskList(Scheduler& scheduler);


#endif
