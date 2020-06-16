all: 
	cls
	g++ -Wall -Wextra -std=c++14 Chip8.cpp -o Chip8 -lSDL2
	# python3 Chip8.py