# Arduino uc32 Basic UI Shield Simulator

The aim of this project is to help students who are learning Arduino, but unfortunately don't have money or are unable to find the hardware to learn on at home. With this simulator students can learn logic required for working with pins, switches, buttons and tasks. Output/Input/Loop are three separate detached tasks using logic which prevents common concurrent problems such as Data Race or Deadlock. This enables real-time response, which makes working with simulator feel like working with the real thing.

## Installing

- [DOWNLOAD](https://github.com/DaniloNovakovic/Arduino_uc32_basic_ui_shield_simulator/archive/master.zip) project from github (you can't copy-paste code from github.com site because it contains special
invisible characters that will not allow your program to compile)

- Create [Visual Studio 2017](https://visualstudio.microsoft.com/downloads/) C++ project and add these files to your project

- When compiling you will need to activate **UNICODE** support (_Project Properties -> General -> \
Character Set -> Use Unicode._). If you forget don't worry, you will get error/warning with adequate message.

- If you get runtime exception upon runing your code, try:
  1) Constructing Simulation class with different parameters (default is: `int width = 80, int height = 40, int fontw = 8, int fonth = 10`)
  2) Changing your console window size in properties settings. (link how: <https://youtu.be/xW8skO7MFYw?t=16m1s>  (shown around 16. min mark))

## Usage

**You write code in _Simulation.cpp_ file**. It is seperated from everything else
so that you can focus only on `setup()` and `loop()` functions, just like when coding in arduino

> Note that i probably won't be making full simulator, i will only be making features required for Embeded Computer Systems class from Applied Software Engineering departement in FTN. Which is basically only L01-L08, SW1-SW4, BTN1-BTN4.

Controls:
  
- 1,2,3,4 - SW4, SW3, SW2, SW1 (on/off) (or SW1,SW2,SW3,SW4 (press F1 to swap between these))
- Q,W,E,R - BTN4, BTN3, BTN2, BTN1 (hold to on, release to off)
- LEDs are ordered in descending order from left to right L08,L07,...,L01

![Console Output Preview](./doc/ArduinoSimulator.JPG)