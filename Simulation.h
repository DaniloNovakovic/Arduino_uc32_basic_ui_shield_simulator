#pragma once
#include "das.h"
#include "tasks.h"

class Simulation : protected ArduinoSimulation_uC32
{
public:
    /**
    Experiment with screen width, height, fontwidth and fontheight until you are satisfied with
    result
    */
    Simulation(int width = 80, int height = 40, int fontw = 8, int fonth = 10) :
        ArduinoSimulation_uC32(width, height, fontw, fonth) {}
public:
    void BeginSimulation() {
        Start();
    }
private:

    virtual void setup() override;
    virtual void loop() override;
};
