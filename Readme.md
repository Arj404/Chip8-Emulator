# CHIP-8 EMULATOR

### **About Chip-8**

CHIP-8 is an interpreted programming language, developed by Joseph Weisbecker. It was initially used on the COSMAC VIP and Telmac 1800 8-bit microcomputers in the mid-1970s. CHIP-8 programs are run on a CHIP-8 virtual machine. It was made to allow video games to be more easily programmed for these computers. These computers typically were designed to use a television as a display, had between 1 and 4K of RAM, and used a 16-key hexadecimal keypad for input. The interpreter took up only 512 bytes of memory, and programs, which were entered into the computer in hexadecimal, were even smaller.<br>
In the early 1990s, the Chip-8 language was revived by a man named Andreas Gustafsson. He created a Chip-8 interpreter for the HP48 graphing calculator, called Chip-48. The HP48 was lacking a way to easily make fast games at the time, and Chip-8 was the answer. Chip-48 later begat Super Chip-48, a modification of Chip-48 which allowed higher resolution graphics, as well as other graphical enhancements.

---

## **Chip-8 Specifications**

### **Memory**

The Chip-8 language is capable of accessing up to 4KB (4,096 bytes) of RAM, from location 0x000 (0) to 0xFFF (4095). The first 512 bytes, from 0x000 to 0x1FF, were reserverd for interpreter.

Memory Map:

<pre>
+---------------+= 0xFFF (4095) End of Chip-8 RAM
|               |
|               |
|               |
|               |
|               |
| 0x200 to 0xFFF|
|     Chip-8    |
| Program / Data|
|     Space     |
|               |
|               |
|               |
+- - - - - - - -+= 0x600 (1536) Start of ETI 660 Chip-8 programs
|               |
|               |
|               |
+---------------+= 0x200 (512) Start of most Chip-8 programs
| 0x000 to 0x1FF|
| Reserved for  |
|  interpreter  |
+---------------+= 0x000 (0) Start of Chip-8 RAM
</pre>

### **Registers**

Chip-8 has 16 general purpose 8-bit registers, usually referred to as Vx, where x is a hexadecimal digit (0 through F). There is also a 16-bit register called I. This register is generally used to store memory addresses, so only the lowest (rightmost) 12 bits are usually used.<br>
The VF register is used as a flag.<br>
Chip-8 also has two special purpose 8-bit registers, for the delay and sound timers. When these registers are non-zero, they are automatically decremented at a rate of 60Hz.<br>
The program counter (PC) is 16-bit, and is used to store the currently executing address. The stack pointer (SP) is 8-bit, it is used to point to the topmost level of the stack.<br>
The stack is an array of 16 16-bit values, used to store the address that the interpreter should return to when finished with a subroutine. Chip-8 allows for up to 16 levels of nested subroutines.

### **Keyboard**

The computers which originally used the Chip-8 Language had a 16-key hexadecimal keypad with the following layout:

<pre>
| 1 | 2 | 3 | C |
| 4 | 5 | 6 | D |
| 7 | 8 | 9 | E |
| A | 0 | B | F |
</pre>

### **Display**

The original implementation of the Chip-8 language used a 64x32-pixel monochrome display. Chip-8 draws graphics on screen through the use of sprites. A sprite is a group of bytes which are a binary representation of the desired picture. Chip-8 sprites may be up to 15 bytes, for a possible sprite size of 8x15.

### **Timers & Sound**

Chip-8 provides 2 timers, a delay timer and a sound timer.<br>
The delay timer is active whenever the delay timer register (DT) is non-zero. This timer does nothing more than subtract 1 from the value of DT at a rate of 60Hz. When DT reaches 0, it deactivates.<br>
The sound timer is active whenever the sound timer register (ST) is non-zero. This timer also decrements at a rate of 60Hz, however, as long as ST's value is greater than zero, the Chip-8 buzzer will sound. When ST reaches zero, the sound timer deactivates.

---

## **Chip-8 Instructions**

Chip-8 language includes 36 different instructions, including math, graphics, and flow control functions. All instructions are 2 bytes long and are stored most-significant-byte first. In memory, the first byte of each instruction should be located at an even addresses. If a program includes sprite data, it should be padded so any instructions following it will be properly situated in RAM.

**_the following variables are used:_**<br> -_nnn or addr - A 12-bit value, the lowest 12 bits of the instruction_<br> -_n or nibble - A 4-bit value, the lowest 4 bits of the instruction_<br> -_x - A 4-bit value, the lower 4 bits of the high byte of the instruction_<br> -_y - A 4-bit value, the upper 4 bits of the low byte of the instruction_<br> -_kk or byte - An 8-bit value, the lowest 8 bits of the instruction_<br>

-**0NNN - SYS addr**<br>
Jump to a machine code routine at nnn.<br><br> -**00E0 - CLS**<br>
Clear the display.<br><br> -**00EE - RET**<br>
Return from a subroutine.<br>
The interpreter sets the program counter to the address at the top of the stack, then subtracts 1 from the stack pointer.<br><br> -**1NNN - JP addr**<br>
Jump to location nnn.<br>
The interpreter sets the program counter to nnn.<br><br> -**2NNN - CALL addr**<br>
Call subroutine at nnn.<br>
The interpreter increments the stack pointer, then puts the current PC on the top of the stack. The PC is then set to nnn.<br><br> -**3XKK - SE Vx, byte**<br>
Skip next instruction if Vx = kk.<br>
The interpreter compares register Vx to kk, and if they are equal, increments the program counter by 2.<br><br> -**4XKK - SNE Vx, byte**<br>
Skip next instruction if Vx != kk.<br>
The interpreter compares register Vx to kk, and if they are not equal, increments the program counter by 2.<br><br> -**5XY0 - SE Vx, Vy**<br>
Skip next instruction if Vx = Vy.<br>
The interpreter compares register Vx to register Vy, and if they are equal, increments the program counter by 2.<br><br> -**6XKK - LD Vx, byte**<br>
Set Vx = kk.<br>
The interpreter puts the value kk into register Vx.<br><br> -**7XKK - ADD Vx, byte**<br>
Set Vx = Vx + kk.<br>
Adds the value kk to the value of register Vx, then stores the result in Vx.<br><br> -**8XY0 - LD Vx, Vy**<br>
Set Vx = Vy.<br>
Stores the value of register Vy in register Vx.<br><br> -**8XY1 - OR Vx, Vy**<br>
Set Vx = Vx OR Vy.<br>
Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx. A bitwise OR compares the corrseponding bits from two values, and if either bit is 1, then the same bit in the result is also 1. Otherwise, it is 0.<br><br> -**8XY2 - AND Vx, Vy**<br>
Set Vx = Vx AND Vy.<br>
Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx. A bitwise AND compares the corrseponding bits from two values, and if both bits are 1, then the same bit in the result is also 1. Otherwise, it is 0.<br><br> -**8XY3 - XOR Vx, Vy**<br>
Set Vx = Vx XOR Vy.<br>
Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx. An exclusive OR compares the corrseponding bits from two values, and if the bits are not both the same, then the corresponding bit in the result is set to 1. Otherwise, it is 0.<br><br> -**8XY4 - ADD Vx, Vy**<br>
Set Vx = Vx + Vy, set VF = carry.<br>
The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx.<br><br> -**8XY5 - SUB Vx, Vy**<br>
Set Vx = Vx - Vy, set VF = NOT borrow.<br>
If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.<br><br> -**8XY6 - SHR Vx {, Vy}**<br>
Set Vx = Vx SHR 1.<br>
If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.<br><br> -**8XY7 - SUBN Vx, Vy**<br>
Set Vx = Vy - Vx, set VF = NOT borrow.<br>
If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.<br><br> -**8XYE - SHL Vx {, Vy}**<br>
Set Vx = Vx SHL 1.<br>
If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.<br><br> -**9XY0 - SNE Vx, Vy**<br>
Skip next instruction if Vx != Vy.<br>
The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.<br><br> -**ANNN - LD I, addr**<br>
Set I = nnn.<br>
The value of register I is set to nnn.<br><br> -**BNNN - JP V0, addr**<br>
Jump to location nnn + V0.<br>
The program counter is set to nnn plus the value of V0.<br><br> -**CXKK - RND Vx, byte**<br>
Set Vx = random byte AND kk.<br>
The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk. The results are stored in Vx. See instruction 8xy2 for more information on AND.<br><br> -**DXYN - DRW Vx, Vy, nibble**<br>
Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.<br>
The interpreter reads n bytes from memory, starting at the address stored in I. These bytes are then displayed as sprites on screen at coordinates (Vx, Vy). Sprites are XORed onto the existing screen. If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0. If the sprite is positioned so part of it is outside the coordinates of the display, it wraps around to the opposite side of the screen. See instruction 8xy3 for more information on XOR, and section 2.4, Display, for more information on the Chip-8 screen and sprites.<br><br> -**EX9E - SKP Vx**<br>
Skip next instruction if key with the value of Vx is pressed.<br>
Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position, PC is increased by 2.<br><br> -**EXA1 - SKNP Vx**<br>
Skip next instruction if key with the value of Vx is not pressed.<br>
Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.<br><br> -**FX07 - LD Vx, DT**<br>
Set Vx = delay timer value.<br>
The value of DT is placed into Vx.<br><br> -**FX0A - LD Vx, K**<br>
Wait for a key press, store the value of the key in Vx.<br>
All execution stops until a key is pressed, then the value of that key is stored in Vx.<br><br> -**FX15 - LD DT, Vx**<br>
Set delay timer = Vx.<br>
DT is set equal to the value of Vx.<br><br> -**FX18 - LD ST, Vx**<br>
Set sound timer = Vx.<br>
ST is set equal to the value of Vx.<br><br> -**FX1E - ADD I, Vx**<br>
Set I = I + Vx.<br>
The values of I and Vx are added, and the results are stored in I.<br><br> -**FX29 - LD F, Vx**<br>
Set I = location of sprite for digit Vx.<br>
The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx. See section 2.4, Display, for more information on the Chip-8 hexadecimal font.<br><br> -**FX33 - LD B, Vx**<br>
Store BCD representation of Vx in memory locations I, I+1, and I+2.<br>
The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.<br><br> -**FX55 - LD [I], Vx**<br>
Store registers V0 through Vx in memory starting at location I.<br>
The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.<br><br> -**FX65 - LD Vx, [I]**<br>
Read registers V0 through Vx from memory starting at location I.<br>
The interpreter reads values from memory starting at location I into registers V0 through Vx.
<br>

## License
This project makes use of an MIT style license. Please see the file called LICENSE.