all: 
	g++ -Wall -Wextra Chip8.cpp -o Chip8 -lSDL2
	python3 Chip8.py