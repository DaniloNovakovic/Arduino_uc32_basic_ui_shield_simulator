#include "tasks.h"

using namespace std;

map<int, Task*> map_tasks;
mutex mx_task;

void deamonTask(int task_id)
{
	unique_lock<mutex> ulock(mx_task);

	auto it_task = map_tasks.find(task_id);
	Task* curr_task = it_task->second;

	while (curr_task != nullptr)
	{
		if (curr_task->stat == TASK_DISABLE)
		{
			do {
				curr_task->cv.wait(ulock);
			} while (curr_task->stat == TASK_DISABLE);
		}

		if (curr_task->stat == TASK_REMOVE)
		{
			map_tasks.erase(it_task);
			delete curr_task;
			return;
		}

		ulock.unlock();

		curr_task->func(task_id, curr_task->var);

		this_thread::sleep_for(chrono::milliseconds(curr_task->period));
		ulock.lock();

		if (curr_task->stat == TASK_RUN_ONCE) {
			curr_task->stat = TASK_DISABLE;
		}
	}
}

void setTaskState(int id, unsigned short state)
{
	unique_lock<mutex> ulock(mx_task);
	auto it_task = map_tasks.find(id);
	if (it_task != map_tasks.end())
	{
		if (it_task->second->stat == TASK_DISABLE && state != TASK_DISABLE) {
			it_task->second->cv.notify_one();
		}
		it_task->second->stat = state;
	}
}
void setTaskPeriod(int id, unsigned long period)
{
	unique_lock<mutex> ulock(mx_task);
	auto it_task = map_tasks.find(id);
	if (it_task != map_tasks.end())
	{
		it_task->second->period = period;
	}
}
void setTaskVar(int id, void* var)
{
	unique_lock<mutex> ulock(mx_task);
	auto it_task = map_tasks.find(id);
	if (it_task != map_tasks.end())
	{
		it_task->second->var = var;
	}
}

int getTaskId(void(*task_func)(int, void*))
{
	unique_lock<mutex> ulock(mx_task);
	for (map<int, Task*>::const_iterator cit = map_tasks.cbegin(); cit != map_tasks.cend(); ++cit)
	{
		if (cit->second->func == task_func)
		{
			return cit->first;
		}
	}
	return -1;
}

unsigned long getTaskPeriod(int id)
{
	unique_lock<mutex> ulock(mx_task);
	auto it = map_tasks.find(id);
	if (it != map_tasks.end())
	{
		return it->second->period;
	}
	return -1;
}

unsigned short getTaskState(int id)
{
	unique_lock<mutex> ulock(mx_task);
	auto it = map_tasks.find(id);
	if (it != map_tasks.end())
	{
		return it->second->stat;
	}
	return -1;
}

void* getTaskVar(int id)
{
	unique_lock<mutex> ulock(mx_task);
	auto it = map_tasks.find(id);
	if (it != map_tasks.end())
	{
		return it->second->var;
	}
	return nullptr;
}

int createTask(void(*task_func)(int, void*), unsigned long task_period, unsigned short task_stat, void* task_var)
{
	int task_id = getTaskId(task_func);
	if (task_id < 0)
	{
		unique_lock<mutex> ulock(mx_task);
		task_id = map_tasks.size() + 1;
		Task* new_task = new Task(task_func, task_period, task_stat, task_var, task_id);
		map_tasks[task_id] = new_task;
		thread{ deamonTask, task_id }.detach();
	}
	return task_id;
}

void destroyTask(int id)
{
	setTaskState(id, TASK_REMOVE);
}