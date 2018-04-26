#include "das.h"

// DEMO: Blink 1 task from FTN-NRS-Applied Software Engineering

using namespace das;

class Simulation : public ArduinoSimulation_uC32 
{
public:
    /**
        Experiment with screen width, height, fontwidth and fontheight until you are satisfiyed with
        result
    */
    Simulation(int width = 80, int height = 40, int fontw = 8, int fonth = 10) : 
        ArduinoSimulation_uC32(width, height, fontw, fonth) {}
public:
    void BeginSimulation() {
        Start();
    }
private:
    
    virtual void setup() override
    {
        for (int i = 26; i <= 33; ++i) {
            pinMode((PIN)i, OUTPUT);
        }
    }

    int leftLed = (int)L08, rightLed = (int) L01;
    int directionLeft = -1, directionRight = 1;

    virtual void loop() override
    {
        delay(100);
        digitalWrite((PIN)leftLed, LOW);
        digitalWrite((PIN)rightLed, LOW);

        if (leftLed == (rightLed + 1) || leftLed <= rightLed) {
            directionLeft = 1;
            directionRight = -1;
        }
        
        if (leftLed == 33 && directionLeft == 1) {
            directionLeft = -1;
        }

        if (rightLed == 26 && directionRight == -1) {
            directionRight = 1;
        }

        leftLed += directionLeft;
        rightLed += directionRight;

        digitalWrite((PIN)leftLed, HIGH);
        digitalWrite((PIN)rightLed, HIGH);
    }

};

int main()
{
    Simulation arduino;
    arduino.BeginSimulation();
    
    return 0;
}

