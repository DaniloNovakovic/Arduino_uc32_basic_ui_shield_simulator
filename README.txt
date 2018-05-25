#UPDATE 5/25/2018
- MAJOR update, i changed code almost completely. digitalRead, digitalWrite etc. are now functions instead of members.
- Added Task support, optimized code a bit and fixed few bugs.
 
TODO (plans for future): To recreate somehow digitalRead, digitalWrite to be more working-with-registers like instead
of current simple bool mPins solution. But this will be work in future because i'm currently busy.
-------------------------------------------------------------------------------------------------------
What you need to do:
- Now if you want to write code you are gonna write it in Simulation.cpp, i seperated it from everything else
so that you can focus only on setup() and loop() functions, just like when coding in arduino

--------------------------------------------------------------------------------------------------------------
## BRIEF / GUIDE##

In my school we are programming this semester in uC32 arduino, but i can't find that chipkit anywhere near me, 
also i couln't find any simulators online, so i decided to make my own simulator, 
and of course all in good old console!  :D Still work in progres...

# Note that i probably won't be making full simulator, i will only be making the stuff i need for school.
Which is basically only L01-L08, SW1-SW4, BTN1-BTN4. 

Controls:
- 1,2,3,4 - SW4, SW3, SW2, SW1 (on/off) (or SW1,SW2,SW3,SW4 (press F1 to swap between these))
- Q,W,E,R - BTN4, BTN3, BTN2, BTN1 (hold to on, release to off)
- LEDs are ordered in descending order from left to right L08,L07,...,L01	

# INSTALLATION GUIDE #

- DOWNLOAD project from github (you can't copy-paste code from github.com site because it contains special
invisible characters that will not allow your program to compile)

- Create Visual Studio 2017 C++ project and add these files to your project

- When compiling you will need to activate UNICODE support (Project Properties -> General -> \
Character Set -> Use Unicode.). If you forget don't worry, you will get error/warning with adequate message.

- If you get runtime exception upon runing your code, try:
	a) Constructing Simulation class with different parameters (default is: int width = 80, int height = 40, int fontw = 8, int fonth = 10)
	b) Changing your console window size in properties settings. (link how: https://youtu.be/xW8skO7MFYw?t=16m1s  (shown around 16. min mark)) 


