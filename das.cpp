#include "das.h"
std::atomic<bool> m_bPins[MAX_UC32_BASIC_IO_PIN + 1];
std::atomic<PIN_MODE> m_PinModes[MAX_UC32_BASIC_IO_PIN + 1];
mutex m_muxPin;

chrono::steady_clock::time_point m_tpSimulationStarted;

/**
Helper function that allows me to iterate through sw pins in a loop
by returning their actual value. (SW1 == swIndex(1))
@example: swIndex(1) will return value 2 (check enum PIN)
*/
int swIndex(int i) {
    Clip(i, 1, NUM_SW);
    switch (i) {
    case 1:
        return (int)SW1;
    case 2:
        return (int)SW2;
    case 3:
        return (int)SW3;
    case 4:
        return (int)SW4;
    default:
        return -1; // not found
    }
}
/**
Helper function that allows me to iterate through btn pins in a loop
by returning their actual value. (BTN1 == btnIndex(1))
@example: btnIndex(1) will return value 4 (check enum PIN)
*/
int btnIndex(int i) {
    Clip(i, 1, NUM_BTN);
    switch (i) {
    case 1:
        return (int)BTN1;
    case 2:
        return (int)BTN2;
    case 3:
        return (int)BTN3;
    case 4:
        return (int)BTN4;
    default:
        return -1; // not found
    }
}


/**
Resets arduino chipset with specific options
@params supported : RUN_SKETCH_ON_BOOT - resets chipset
note: ENTER_BOOTLOADER_ON_BOOT is not yet implemented
*/
void executeSoftReset(uint32_t options) 
{
    if (options == RUN_SKETCH_ON_BOOT) 
    {
        for (int i = 0; i < MAX_UC32_BASIC_IO_PIN; ++i) {
            m_bPins[i].store(false);
        }
        m_tpSimulationStarted = chrono::steady_clock::now();
    }
    else if (options == ENTER_BOOTLOADER_ON_BOOT) {
        // ...
    }
}


/**
Sets pin to certain mode (INPUT, OUTPUT, INPUT_PULLUP)
@note: INPUT_PULLUP support is not yet implemented.
It doesn't do anything in this current state of simulation class.
*/
void pinMode(int pin, PIN_MODE mode) {
    pinMode((PIN)pin, mode);
}
void pinMode(PIN pin, PIN_MODE mode) {
    m_PinModes[(int)pin].store(mode);
}

/**
Sets given 'pin' to 'value' (LOW, HIGH)
@note: 'pin' given in params must be previously set to OUTPUT before
calling this method. If it wasn't set on OUTPUT mode method will throw exception.
(exception won't be thrown if m_bLearningMode is set to off)
*/
void digitalWrite(int pin, int value) {
    /*if (pin < 0 || pin > MAX_UC32_BASIC_IO_PIN) {
    wchar_t errBuff[100];
    wsprintf(errBuff, L" \"int digitalWrite(%d)\" : argument \"%d\" is invalid!", pin, pin);
    exit(Error(errBuff));
    }
    else {*/
    digitalWrite((PIN)pin, value);
    //}
}


void digitalWrite(PIN pin, int value)
{
    /*if (m_bLearningMode && m_PinModes[(int)pin].load() != PIN_MODE::OUTPUT)
    {
    exit(Error(L"error: you called digital write on pin that had no OUTPUT mode set!"));
    }*/
    m_bPins[(int)pin].store(value > 0);
}

/**
Reads the state of pin. (ON/OFF)
*/
int digitalRead(int pin) {
    if (pin < 0 || pin > MAX_UC32_BASIC_IO_PIN) {
        wchar_t errBuff[100];
        wsprintf(errBuff, L" \"int digitalRead(%d)\" : argument \"%d\" is invalid!", pin, pin);
        //exit(Error(errBuff));
        exit(EXIT_FAILURE);
    }
    else {
        return m_bPins[pin].load() == true ? HIGH : LOW;
    }
}

int digitalRead(PIN pin) {
    return m_bPins[(int)pin].load() == true ? HIGH : LOW;
}

void delay(unsigned int ms) {
    this_thread::sleep_for(chrono::milliseconds(ms));
}

/**
Returns number less then 'howbig'
*/
long random(long howbig)
{
    if (howbig > 0) {
        return random(0, howbig - 1);
    }
    return 0;
}

/**
Returns number from closed interval [howsmall, howbig]
*/
long random(long howsmall, long howbig)
{
    if (howsmall > howbig) {
        std::swap(howsmall, howbig);
    }
    static default_random_engine generator((unsigned int)chrono::system_clock::now().time_since_epoch().count());
    uniform_int_distribution<long> distribution(howsmall, howbig);
    return distribution(generator);
}

/**
Returns number of milliseconds passed from the start of the simulation
*/
long long millis()
{

    auto timePassed = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - m_tpSimulationStarted);
    return timePassed.count();
}

/**
Swaps variables with XOR (withouth bonus variable)
*/
static void Swap(int &x, int &y) {
    x ^= y;
    y ^= x;
    x ^= y;
}

/**
Rounds 'value' to closed interval ['lowerBoundary', 'higherBoundary']
*/
static void Clip(int &value, int lowerBoundary, int higherBoundary) {
    if (lowerBoundary > higherBoundary) {
        Swap(lowerBoundary, higherBoundary);
    }
    if (value < lowerBoundary) {
        value = lowerBoundary;
    }
    else if (value > higherBoundary) {
        value = higherBoundary;
    }
}
