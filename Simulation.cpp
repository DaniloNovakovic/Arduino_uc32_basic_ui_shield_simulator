// This is a demo app to show you how you can use the Simulator

#include "Simulation.h"

using namespace das;

#define PIN_LED1 33
#define PIN_LED2 27
#define PIN_SW2   7

int blink1_id, blink2_id;
unsigned long blink1_var;

unsigned short stat2 = TASK_DISABLE;
unsigned long period1, period2;

void blink_task1(int id, void* tptr)
{
    digitalWrite(PIN_LED1, !digitalRead(PIN_LED1));   // Toggle pin state
}

void blink_task2(int id, void* tptr)
{
    digitalWrite(PIN_LED2, !digitalRead(PIN_LED2));   // Toggle pin state
}

void blink_task3(int id, void* tptr)
{
    stat2 = getTaskState(blink2_id);
    if (digitalRead(PIN_SW2))
    {
        // SW2=ON, pokreni task2
        if (stat2 == TASK_DISABLE)
        {
            // postavi RANDOM vreme rasporedjivanja
            int rTime = random(1, 10) * 25;
            setTaskPeriod(blink2_id, rTime);
            setTaskState(blink2_id, TASK_ENABLE);
        }
    }
    else
    {
        if (stat2 == TASK_ENABLE)
        {
            setTaskState(blink2_id, TASK_DISABLE);
            digitalWrite(PIN_LED2, LOW);
        }
    }
}

void Simulation::setup()
{
    // initialize the digital pin as an output.
    pinMode(PIN_LED1, OUTPUT);
    pinMode(PIN_LED2, OUTPUT);

    //create blink tasks
    blink1_id = createTask(blink_task1, 500, TASK_ENABLE, &blink1_var);
    blink2_id = createTask(blink_task2, 25, stat2, NULL);
    createTask(blink_task3, 1000, TASK_ENABLE, NULL);
}

#define BTN4 37
bool btn4, do_reset;
long long time1;

void Simulation::loop()
{
    delay(100);
    if (digitalRead(BTN4))
    {
        if (!btn4)
        {
            time1 = millis();     // oznaci vreme prve promene
            btn4 = true;          // tranzicija 0->1
        }
        else
        {
            if (millis() - time1 > 2000)
                do_reset = true;
        }
    }
    else
    {
        btn4 = false;
        // uradi reset ako treba
        if (do_reset) {
            executeSoftReset(RUN_SKETCH_ON_BOOT);
            do_reset = false;
        }
    }
}