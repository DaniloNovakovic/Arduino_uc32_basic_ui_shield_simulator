#pragma once
#include "olcConsoleGameEngine.h"
#include <random>
#include <algorithm>

/**
	Reason why i made custom namespace is because INPUT is already defined in windows.h, compiler gives me errors
	And the reason why i didn't put everything in das namespace is because i have a linker error then with Simulation.cpp
	file which i don't know how to solve. :/
	So this will be current solution to my problem, even tho i do not like it. I would prefer if everything was inside one namespace, but oh well... :/
*/
namespace das {
	enum PIN
	{
		L01 = 26, L02, L03, L04, L05, L06, L07, L08,
		SW1 = 2, SW2 = 7, SW3 = 8, SW4 = 35,
		BTN1 = 4, BTN2 = 34, BTN3 = 36, BTN4 = 37
	};

	enum PIN_MODE {
		INPUT = 0, OUTPUT, INPUT_PULLUP
	};
}

using namespace das;

const int RUN_SKETCH_ON_BOOT(0), ENTER_BOOTLOADER_ON_BOOT(1);

const int LOW(0);
const int HIGH(5);  // approx 5 volts

// Feel free to add more pins if you like, for my needs L01-L08, SW1-SW4 and BTN1-BTN4 are enough
const int MAX_UC32_BASIC_IO_PIN(37);
const int NUM_SW(4);
const int NUM_LED(8);
const int NUM_BTN(4);

// global fields
extern std::atomic<bool> m_bPins[MAX_UC32_BASIC_IO_PIN + 1];
extern std::atomic<PIN_MODE> m_PinModes[MAX_UC32_BASIC_IO_PIN + 1];
extern mutex m_muxPin;

extern chrono::steady_clock::time_point m_tpSimulationStarted;

/**
Rounds 'value' to closed interval ['lowerBoundary', 'higherBoundary']
*/
void Clip(int& value, int lowerBoundary, int higherBoundary);

/**
Swaps variables with XOR (withouth bonus variable)
*/
static void Swap(int& x, int& y);

/**
Returns number of milliseconds passed from the start of the simulation
*/
long long millis();

/**
Returns number less then 'howbig'
*/
long random(long howbig);

/**
Returns number from closed interval [howsmall, howbig]
*/
long random(long howsmall, long howbig);

void delay(unsigned int ms);

/**
Reads the state of pin. (ON/OFF)
*/
int digitalRead(int pin);
int digitalRead(PIN pin);

/**
Sets given 'pin' to 'value' (LOW, HIGH)
@note: 'pin' given in params must be previously set to OUTPUT before
calling this method. If it wasn't set on OUTPUT mode method will throw exception.
(exception won't be thrown if m_bLearningMode is set to off)
*/
void digitalWrite(int pin, int value);
void digitalWrite(PIN pin, int value);

/**
Sets pin to certain mode (INPUT, OUTPUT, INPUT_PULLUP)
@note: INPUT_PULLUP support is not yet implemented.
It doesn't do anything in this current state of simulation class.
*/
void pinMode(int pin, PIN_MODE mode);
void pinMode(PIN pin, PIN_MODE mode);

/**
Resets arduino chipset with specific options
@params supported : RUN_SKETCH_ON_BOOT - resets chipset
note: ENTER_BOOTLOADER_ON_BOOT is not yet implemented
*/
void executeSoftReset(uint32_t options);

/**
Helper function that allows me to iterate through btn pins in a loop
by returning their actual value. (BTN1 == btnIndex(1))
@example: btnIndex(1) will return value 4 (check enum PIN)
*/
int btnIndex(int i);

/**
Helper function that allows me to iterate through sw pins in a loop
by returning their actual value. (SW1 == swIndex(1))
@example: swIndex(1) will return value 2 (check enum PIN)
*/
int swIndex(int i);

/**
	Simulation of Arduino uC32 - Basic IO Shield, used for educational purposes.

	@note: New Console window is created in ArduinoSimulation_uC32 constructor
	with default parameters that can potentionally cause exception depending on your
	settings. To change these, class that ihnerits ArduinoSimulation_uC32 will have
	to call its constructor with different parameters.

	@author Danilo Novakoviæ
	@version 0.5.1   5/25/2018
*/
class ArduinoSimulation_uC32 : private olcConsoleGameEngine
{
protected:
	// You MUST implement these!!!
	virtual void setup() = 0;
	virtual void loop() = 0;
	void Start() {
		olcConsoleGameEngine::Start();
	}
private:
	struct sRectangleObject
	{
		short width, height;
		sRectangleObject(short width = 1, short height = 2) {
			set(width, height);
		}
		void set(short width, short height) {
			this->width = width;
			this->height = height;
		}
	};

	sRectangleObject m_swModel, m_btnModel, m_ledModel;
private:
	/**
		@note: Set 'm_bLearningMode' to off if you don't wish to be thrown unecessary exceptions
		or/and possible warnings aimed to serve as reminder to not forget to, for example, set
		pinModes to on/off. (Even though simulation will work withouth doing it)
	*/
	bool m_bLearningMode;
public:
	ArduinoSimulation_uC32(int width = 80, int height = 60, int fontw = 6, int fonth = 6) {
		m_sAppName = L"ArduinoSimulation_uC32 by Danilo Novakovic";
		try {
			ConstructConsole(width, height, fontw, fonth);
		}
		catch (...) {
			printf_s("\nError: Unable to construct console window with width=%d, height=%d, fotnw=%d, fonth=%d\n",
				width, height, fontw, fonth);
			exit(EXIT_FAILURE);
		}
		m_bLearningMode = true;
	}
	~ArduinoSimulation_uC32() {}

private:
	// Inherited via olcConsoleGameEngine
	virtual bool OnUserCreate() override
	{
		m_swModel.set(5, 15);
		m_btnModel.set(5, 5);
		m_ledModel.set(4, 8);

		m_tpSimulationStarted = chrono::steady_clock::now();

		executeSoftReset(RUN_SKETCH_ON_BOOT);
		setup();

		thread{ [&]() {HandleInput(); } }.detach();
		thread{ [&]() {HandleOutput(); } }.detach();

		return true;
	}
	virtual bool OnUserUpdate(float fElapsedTime) override
	{
		loop();

		return true;
	}

	void HandleInput()
	{
		unique_lock<mutex> ulock(m_muxPin);
		while (1)
		{
			HandleKeyboardInput();
			HandleInputChipKit();

			ulock.unlock();
			this_thread::sleep_for(chrono::milliseconds(10));   // ~ 65 times per second
			ulock.lock();
		}
	}
	void HandleOutput()
	{
		unique_lock<mutex> ulock(m_muxPin);
		while (1)
		{
			DrawChipKIT();
			UpdateScreen();

			ulock.unlock();
			this_thread::sleep_for(chrono::milliseconds(16)); // ~ 60fps
			ulock.lock();
		}
	}

	void HandleInputChipKit()
	{
		// HANDLE INPUT
		if (m_keys[VK_ESCAPE].bReleased) {
			exit(EXIT_SUCCESS);
		}

		static bool bToogledF1 = false;

		if (m_keys[VK_F1].bReleased) {
			bToogledF1 = !bToogledF1;
		}

		// keys 1,2,3,4 are reserved for SW1,SW2,SW3,SW4 (or SW4,SW3,SW2,SW1 based on bToogledF1)
		for (int i = 1; i <= NUM_SW; ++i)
		{
			if (m_keys[i + '0'].bReleased) {
				int index = bToogledF1 == true ? swIndex(NUM_SW - i + 1) : swIndex(i);
				m_bPins[index].store(!m_bPins[index].load());
			}
		}

		// keyboard chars Q,W,E,R are reserved for BTN flips (on-off)
		for (int i = 0; i < NUM_BTN; ++i)
		{
			if (m_keys["QWER"[i]].bHeld || m_keys["qwer"[i]].bHeld) {
				m_bPins[btnIndex(NUM_BTN - i)].store(true);
			}
			else {
				m_bPins[btnIndex(NUM_BTN - i)].store(false);
			}
		}
	}

	/**
		Updates simulated chipkit (pins) on the console screen.
	*/
	void DrawChipKIT()
	{
		int y = 2, x = 1, i;
		static const int SPACE_BETWEEN_PINS(1);
		COLOUR colour;
		PIXEL_TYPE type;

		// Draw SW's
		for (i = 1, x = 1; x < ScreenWidth() && i <= NUM_SW; ++i)
		{
			if (m_bPins[swIndex(i)].load()) {
				colour = FG_GREEN;
				type = PIXEL_SOLID;
			}
			else {
				colour = FG_RED;
				type = PIXEL_HALF;
			}

			Fill(x, y, x + m_swModel.width, y + m_swModel.height, type, colour);
			x += m_swModel.width + SPACE_BETWEEN_PINS;
		}
		y += m_swModel.height + SPACE_BETWEEN_PINS * 3;

		// Draw BTNs
		for (i = 0, x = 1; x < ScreenWidth() && i < NUM_BTN; ++i)
		{
			type = m_bPins[btnIndex(NUM_BTN - i)].load() == true ? PIXEL_SOLID : PIXEL_QUARTER;
			Fill(x, y, x + m_btnModel.width, y + m_btnModel.height, type, FG_GREY);
			x += m_btnModel.width + SPACE_BETWEEN_PINS;
		}

		// Draw LEDs
		for (i = 0, x += m_btnModel.width; x < ScreenWidth() && i < NUM_LED; ++i)
		{
			if (m_bPins[(int)PIN::L08 - i].load()) {
				colour = FG_GREEN;
				type = PIXEL_SOLID;
			}
			else {
				colour = FG_RED;
				type = PIXEL_HALF;
			}

			Fill(x, y, x + m_ledModel.width, y + m_ledModel.height, type, colour);
			x += m_ledModel.width + SPACE_BETWEEN_PINS;
		}
	}
};
