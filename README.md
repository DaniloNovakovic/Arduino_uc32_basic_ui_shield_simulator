# Arduino uc32 Basic UI Shield Simulator
The aim of this project is to help students who are learning Arduino, but unfortunately don't have money or are unable to find the hardware to learn on at home. With this simulator students can learn logic required for working with pins, switches, buttons and tasks. Output/Input/Loop are three separate detached tasks using logic which prevents common concurrent problems such as Data Race or Deadlock. This enables real-time response, which makes working with simulator feel like working with the real thing.

## Installing 
- DOWNLOAD project from github (you can't copy-paste code from github.com site because it contains special
invisible characters that will not allow your program to compile)

- Create Visual Studio 2017 C++ project and add these files to your project

- When compiling you will need to activate UNICODE support (Project Properties -> General -> \
Character Set -> Use Unicode.). If you forget don't worry, you will get error/warning with adequate message.

- If you get runtime exception upon runing your code, try:
	a) Constructing Simulation class with different parameters (default is: int width = 80, int height = 40, int fontw = 8, int fonth = 10)
	b) Changing your console window size in properties settings. (link how: https://youtu.be/xW8skO7MFYw?t=16m1s  (shown around 16. min mark)) 

## Usage
- Now if you want to write code you are gonna write it in Simulation.cpp, i seperated it from everything else
so that you can focus only on setup() and loop() functions, just like when coding in arduino

> Note that i probably won't be making full simulator, i will only be making the stuff i need for school. Which is basically only L01-L08, SW1-SW4, BTN1-BTN4. 

- Controls:
	- 1,2,3,4 - SW4, SW3, SW2, SW1 (on/off) (or SW1,SW2,SW3,SW4 (press F1 to swap between these))
	- Q,W,E,R - BTN4, BTN3, BTN2, BTN1 (hold to on, release to off)
	- LEDs are ordered in descending order from left to right L08,L07,...,L01	



