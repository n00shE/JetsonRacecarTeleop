all:
	g++ teleop.cpp ./src/JHPWMPCA9685.cpp ./src/joystick.cc -I./src --std=c++11 -o teleop
