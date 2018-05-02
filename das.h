#pragma once
#include "olcConsoleGameEngine.h"
#include <random>
#include <algorithm>

// Danilo's Arduino Simulation namespace 

namespace das 
{   
    /**
        Swaps variables with XOR (withouth bonus variable)
    */
    void Swap(int &x, int &y) {
        x ^= y;
        y ^= x;
        x ^= y;
    }

    /**
    Rounds 'value' to closed interval ['lowerBoundary', 'higherBoundary']
    */
    void Clip(int &value, int lowerBoundary, int higherBoundary) {
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

    const int RUN_SKETCH_ON_BOOT(0), ENTER_BOOTLOADER_ON_BOOT(1);

    const int LOW(0);
    const int HIGH(5);  // approx 5 volts 

    // Feel free to add more pins if you like, for my needs L01-L08, SW1-SW4 and BTN1-BTN4 are enough
    const int MAX_UC32_BASIC_IO_PIN(37);
    const int NUM_SW(4);
    const int NUM_LED(8);
    const int NUM_BTN(4);

    enum PIN 
    {
        L01 = 26, L02, L03, L04, L05, L06, L07, L08, 
        SW1 = 2, SW2 = 7, SW3 = 8, SW4 = 35, 
        BTN1 = 4, BTN2 = 34, BTN3 = 36, BTN4 = 37
    };

    enum PIN_MODE {
        INPUT = 0, OUTPUT, INPUT_PULLUP
    };
    const int NUM_DEAMON_THREADS = 3;

    /**
        Simulation of Arduino uC32 - Basic IO Shield, used for educational purposes.

        @note1: This is an abstract class with abstract methods 'void setup()' and 'void loop()'
        you MUST implement these. Also you need to call method Start() which is protected
        and inherited from olcConsoleGameEngine.h to start the simulation.

        @note2: New Console window is created in ArduinoSimulation_uC32 constructor
        with default parameters that can potentionally cause exception depending on your 
        settings. To change these, class that ihnerits ArduinoSimulation_uC32 will have
        to call its constructor with different parameters.

        @author Danilo Novakoviæ
        @version 0.4.2   5/2/2018
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

        // fields
        std::atomic<bool> m_bPins[MAX_UC32_BASIC_IO_PIN + 1];
        std::atomic<PIN_MODE> m_PinModes[MAX_UC32_BASIC_IO_PIN + 1];
        sRectangleObject m_swModel, m_btnModel, m_ledModel;

        const int THREAD_INPUT_ID = 0, THREAD_LOOP_ID = 1, THREAD_OUTPUT_ID = 2;
        std::atomic<int> m_activeThread;
        std::atomic<int> m_readyThreads[NUM_DEAMON_THREADS];
        condition_variable m_cvDeamonThreads[NUM_DEAMON_THREADS];
        condition_variable m_cvScheduler;
        mutex m_muxPin;

        chrono::steady_clock::time_point m_tpSimulationStarted;
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

            m_activeThread = -1;
            for (int i = 0; i < NUM_DEAMON_THREADS; ++i) {
                m_readyThreads[i] = 0;
            }

            m_tpSimulationStarted = chrono::steady_clock::now();

            executeSoftReset(RUN_SKETCH_ON_BOOT);
            setup();

            thread{ [&]() {HandleInput(); } }.detach();
            thread{ [&]() {HandleLoop(); } }.detach();
            thread{ [&]() {HandleOutput(); } }.detach();

            return true;
        }
        virtual bool OnUserUpdate(float fElapsedTime) override
        {
            Scheduler();

            return true;
        }
        void Scheduler() 
        {
            unique_lock<mutex> ulock(m_muxPin);
            while (1) 
            {
                m_activeThread = modRobin(m_activeThread + 1);
                if (m_readyThreads[m_activeThread] > 0) {
                    m_cvDeamonThreads[m_activeThread].notify_one();
                    m_cvScheduler.wait(ulock);
                }
            }

        }
        const int TESTING_WAIT_TIME = 100;
        void HandleInput() 
        {
            unique_lock<mutex> ulock(m_muxPin);
            while (1) 
            {
                do 
                {
                    ++m_readyThreads[THREAD_INPUT_ID];
                    m_cvDeamonThreads[THREAD_INPUT_ID].wait(ulock);
                    --m_readyThreads[THREAD_INPUT_ID];
                } while (m_activeThread != THREAD_INPUT_ID);

                HandleKeyboardInput();
                //HandleMouseInput();
                HandleInputChipKit();

                m_cvScheduler.notify_one();

            }
        }
        void HandleOutput() 
        {
            unique_lock<mutex> ulock(m_muxPin);
            while (1) 
            {
                do
                {
                    ++m_readyThreads[THREAD_OUTPUT_ID];
                    m_cvDeamonThreads[THREAD_OUTPUT_ID].wait(ulock);
                    --m_readyThreads[THREAD_OUTPUT_ID];
                } while (m_activeThread != THREAD_OUTPUT_ID);

                DrawChipKIT();
                UpdateScreen();

                m_cvScheduler.notify_one();
            }
        }
        void HandleLoop() 
        {
            unique_lock<mutex> ulock(m_muxPin);
            while (1) 
            {
                do
                {
                    ++m_readyThreads[THREAD_LOOP_ID];
                    m_cvDeamonThreads[THREAD_LOOP_ID].wait(ulock);
                    --m_readyThreads[THREAD_LOOP_ID];
                } while (m_activeThread != THREAD_LOOP_ID);

                ulock.unlock();
                loop();
                ulock.lock();

                m_cvScheduler.notify_one();
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
            This function was used as prototype for "delay(unsigned int ms)" function that user will call.
            Once i create new feature i experiment with delayTest function, and once i'm sure it works as intented
            i then update delay(unsigned int ms) properly.
        */
        void delayTest(unique_lock<mutex>& ulock, unsigned int ms, const wchar_t* debugMsg) 
        {
            DrawString(0, m_activeThread, debugMsg);
            UpdateScreen();

            int thread_id = m_activeThread;
            
            ulock.unlock();
            m_cvScheduler.notify_one();

            this_thread::sleep_for(chrono::milliseconds(ms));
            ulock.lock();
            
            while(m_activeThread != thread_id) {
                ++m_readyThreads[thread_id];
                m_cvDeamonThreads[thread_id].wait(ulock);
                --m_readyThreads[thread_id];
            }
        }

        int modRobin(int x, const int size = NUM_DEAMON_THREADS) {
            return x >= size ? 0 : x;
        }
        /**
            Helper function that allows me to iterate through sw pins in a loop
            by returning their actual value. (SW1 == swIndex(1))
            @example: swIndex(1) will return value 2 (check enum das::PIN)
        */
        int swIndex(int i) {
            das::Clip(i, 1, NUM_SW);
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
        @example: btnIndex(1) will return value 4 (check enum das::PIN)
        */
        int btnIndex(int i) {
            das::Clip(i, 1, NUM_BTN);
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
    public:
        /**
            Resets arduino chipset with specific options
            @params supported : RUN_SKETCH_ON_BOOT - resets chipset 
            note: ENTER_BOOTLOADER_ON_BOOT is not yet implemented
        */
        void executeSoftReset(uint32_t options) {
            if (options == RUN_SKETCH_ON_BOOT) {
                for (int i = 0; i < MAX_UC32_BASIC_IO_PIN; ++i) {
                    m_bPins[i].store(false);
                }
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
            if (pin < 0 || pin > MAX_UC32_BASIC_IO_PIN) {
                wchar_t errBuff[100];
                wsprintf(errBuff, L" \"int digitalWrite(%d)\" : argument \"%d\" is invalid!", pin, pin);
                exit(Error(errBuff));
            }
            else {
                digitalWrite((PIN)pin, value);
            }
        }
        void digitalWrite(PIN pin, int value) 
        {
            if (m_bLearningMode && m_PinModes[(int)pin].load() != PIN_MODE::OUTPUT) 
            {
                exit(Error(L"error: you called digital write on pin that had no OUTPUT mode set!"));
            }
            m_bPins[(int)pin].store(value > 0);
        }

        /**
            Reads the state of pin. (ON/OFF)
        */
        int digitalRead(int pin) {
            if (pin < 0 || pin > MAX_UC32_BASIC_IO_PIN) {
                wchar_t errBuff[100];
                wsprintf(errBuff, L" \"int digitalRead(%d)\" : argument \"%d\" is invalid!", pin, pin);
                exit(Error(errBuff));
            }
            else {
                return m_bPins[pin].load() == true ? HIGH : LOW;
            }
        }
        int digitalRead(PIN pin) {
            return m_bPins[(int)pin].load() == true ? HIGH : LOW;
        }

        /**
            @note: this function will attempt to lock(m_muxPin), so if you already hold 
            that mutex you will have to unlock it first before calling this function
        */
        void delay(unsigned int ms) {
            delay(ms, nullptr);
        }

    protected:
        /**
        @note: this version of delay can be used for debuging, although it is not supported by
        Arduino (it is custom made)
        */
        void delay(unsigned int ms, const wchar_t* debug_msg)
        {
            unique_lock<mutex> ulock(m_muxPin);
            int thread_id = m_activeThread;

            if (debug_msg != nullptr) {
                DrawString(0, m_activeThread, debug_msg);
            }
            ulock.unlock();
            m_cvScheduler.notify_one();

            this_thread::sleep_for(chrono::milliseconds(ms));
            ulock.lock();

            while (m_activeThread != thread_id) {
                ++m_readyThreads[thread_id];
                m_cvDeamonThreads[thread_id].wait(ulock);
                --m_readyThreads[thread_id];
            }
        }
    public:
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
            static default_random_engine generator((unsigned int) chrono::system_clock::now().time_since_epoch().count());
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
   
    
    };
    
    

}