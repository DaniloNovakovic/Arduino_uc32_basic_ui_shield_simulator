#include "das.h"
std::atomic<bool> m_bPins[MAX_UC32_BASIC_IO_PIN + 1];
std::atomic<PIN_MODE> m_PinModes[MAX_UC32_BASIC_IO_PIN + 1];
mutex m_muxPin;

chrono::steady_clock::time_point m_tpSimulationStarted;

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
    }
}

void pinMode(int pin, PIN_MODE mode) {
    pinMode((PIN)pin, mode);
}
void pinMode(PIN pin, PIN_MODE mode) {
    m_PinModes[(int)pin].store(mode);
}

void digitalWrite(int pin, int value) {
    digitalWrite((PIN)pin, value);
}

void digitalWrite(PIN pin, int value)
{
    m_bPins[(int)pin].store(value > 0);
}

int digitalRead(int pin) {
    if (pin < 0 || pin > MAX_UC32_BASIC_IO_PIN) {
        wchar_t errBuff[100];
        wsprintf(errBuff, L" \"int digitalRead(%d)\" : argument \"%d\" is invalid!", pin, pin);
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

long random(long howbig)
{
    if (howbig > 0) {
        return random(0, howbig - 1);
    }
    return 0;
}

long random(long howsmall, long howbig)
{
    if (howsmall > howbig) {
        std::swap(howsmall, howbig);
    }
    static default_random_engine generator((unsigned int)chrono::system_clock::now().time_since_epoch().count());
    uniform_int_distribution<long> distribution(howsmall, howbig);
    return distribution(generator);
}

long long millis()
{
    auto timePassed = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - m_tpSimulationStarted);
    return timePassed.count();
}

static void Swap(int& x, int& y) {
    x ^= y;
    y ^= x;
    x ^= y;
}

static void Clip(int& value, int lowerBoundary, int higherBoundary) {
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