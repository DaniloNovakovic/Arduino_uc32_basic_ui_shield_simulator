#pragma once
#include "olcConsoleGameEngine.h"

// Danilo's Arduino Simulation namespace 

namespace das 
{   
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
        @version 0.1   4/25/2018
    */
    class ArduinoSimulation_uC32 : protected olcConsoleGameEngine
    {
    protected:
        // You MUST implement these!!!
        virtual void setup() = 0;
        virtual void loop() = 0;
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
        bool m_bPins[MAX_UC32_BASIC_IO_PIN + 1];
        PIN_MODE m_PinModes[MAX_UC32_BASIC_IO_PIN + 1];
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

            executeSoftReset(RUN_SKETCH_ON_BOOT);
            setup();
            return true;
        }
        virtual bool OnUserUpdate(float fElapsedTime) override
        {
            // HANDLE INPUT
            if (m_keys[VK_ESCAPE].bReleased == true) {
                return false;
            }

            for (int i = 1; i <= NUM_SW; ++i) {
                // keyboard nums 1-4 are reserved for SW flips (on-off)
                if (m_keys[i + '0'].bReleased == true) {
                    int index = swIndex(NUM_SW - i + 1);
                    m_bPins[index] = !m_bPins[index];
                }
            }

            for (int i = 0; i < NUM_BTN; ++i) {
                // keyboard chars Q,W,E,R are reserved for BTN flips (on-off)
                if (m_keys["QWER"[i]].bHeld || m_keys["qwer"[i]].bHeld) {
                    m_bPins[btnIndex(NUM_BTN - i)] = true;
                }
                else {
                    m_bPins[btnIndex(NUM_BTN - i)] = false;
                }
            }

            // USER FUNCTION
            loop();
            
            // UPDATE SCREEN
            DrawChipKIT();

            return true;
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
            // Draw SW's 
            int y = 2, x = 1, i;
            static const int SPACE_BETWEEN_PINS(1);
            COLOUR colour;

            for (i = 1, x = 1; x < ScreenWidth() && i <= NUM_SW; ++i) 
            {
                colour = m_bPins[swIndex(i)] == true ? FG_GREEN : FG_RED;
                Fill(x, y, x + m_swModel.width, y + m_swModel.height, PIXEL_SOLID, colour);
                x += m_swModel.width + SPACE_BETWEEN_PINS;
            }
            y += m_swModel.height + SPACE_BETWEEN_PINS * 3;

            // Draw BTNs
            for (i = 0, x = 1; x < ScreenWidth() && i < NUM_BTN; ++i) 
            {
                PIXEL_TYPE type = m_bPins[btnIndex(NUM_BTN - i)] == true ? PIXEL_SOLID : PIXEL_QUARTER;
                Fill(x, y, x + m_btnModel.width, y + m_btnModel.height, type, FG_GREY);
                x += m_btnModel.width + SPACE_BETWEEN_PINS;
            }

            // Draw LEDs
            for (i = 0, x += m_btnModel.width; x < ScreenWidth() && i < NUM_LED; ++i) 
            {
                colour = m_bPins[(int)PIN::L08 - i] == true ? FG_GREEN : FG_RED;
                Fill(x, y, x + m_ledModel.width, y + m_ledModel.height, PIXEL_SOLID, colour);
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
                    m_bPins[i] = false;
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
        void pinMode(PIN pin, PIN_MODE mode) {
            m_PinModes[(int)pin] = mode;
        }
        
        /**
            Sets given 'pin' to 'value' (LOW, HIGH)
            @note: 'pin' given in params must be previously set to OUTPUT before
            calling this method. If it wasn't set on OUTPUT mode method will throw exception.
            (exception won't be thrown if m_bLearningMode is set to off)
        */
        void digitalWrite(PIN pin, int value) 
        {
            if (m_bLearningMode && m_PinModes[(int)pin] != PIN_MODE::OUTPUT) 
            {
                exit(Error(L"error: you called digital write on pin that had no OUTPUT mode set!"));
            }
            m_bPins[(int)pin] = value > 0;
        }

        /**
            Reads the state of pin. (ON/OFF)
        */
        int digitalRead(PIN pin) {
            return m_bPins[(int)pin] == true ? LOW : HIGH;
        }
        void delay(unsigned int ms) 
        {
            // I need to optimize this to work a bit better
            DrawChipKIT();
            thread t([this](){ this->UpdateScreen(); });
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
            t.join();
        }
    
    };
    
    

}