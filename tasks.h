#pragma once
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <map>
#include <chrono>

enum TaskStates { TASK_ENABLE, TASK_DISABLE, TASK_RUN_ONCE, TASK_REMOVE };

struct Task
{
    int id;
    void *var;
    unsigned long period;
    unsigned short stat;
    std::condition_variable cv;

    void(*func)(int, void *);
    Task(void(*task_func)(int, void*), unsigned long task_period,
        unsigned short task_stat, void *task_var = nullptr, int task_id = -1)
    {
        func = task_func;
        id = task_id;
        var = task_var;
        stat = task_stat;
        period = task_period;
    }
};

extern std::map<int, Task*> map_tasks;
extern std::mutex mx_task;

void deamonTask(int task_id);
void setTaskState(int id, unsigned short state);
void setTaskPeriod(int id, unsigned long period);
void setTaskVar(int id, void *var);
int getTaskId(void(*task_func)(int, void *));
unsigned long getTaskPeriod(int id);
unsigned short getTaskState(int id);
void *getTaskVar(int id);
int createTask(void(*task_func)(int, void *), unsigned long task_period, unsigned short task_stat, void *task_var);
void destroyTask(int id);
