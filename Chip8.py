#############################################################################
## Name: Chip8 VM
##
## Author: Arjav Jain
## Contact: arjavjain598@gmail.com
##
## License: MIT style license
##############################################################################
import os


class Chip8(object):
    gfx = [0]*64*32
    key = [0]*16
    drawFlag = True
    __memory = [0]*4096
    __delayTimer = 0
    __soundTimer = 0
    __v = [0]*16
    __stack = [0]*16
    __i = 0
    __pc = 0
    __opcode = 0
    __sp = 0
    __fontset = [0xF0, 0x90, 0x90, 0x90, 0xF0,
                 0x20, 0x60, 0x20, 0x20, 0x70,
                 0xF0, 0x10, 0xF0, 0x80, 0xF0,
                 0xF0, 0x10, 0xF0, 0x10, 0xF0,
                 0x90, 0x90, 0xF0, 0x10, 0x10,
                 0xF0, 0x80, 0xF0, 0x10, 0xF0,
                 0xF0, 0x80, 0xF0, 0x90, 0xF0,
                 0xF0, 0x10, 0x20, 0x40, 0x40,
                 0xF0, 0x90, 0xF0, 0x90, 0xF0,
                 0xF0, 0x90, 0xF0, 0x10, 0xF0,
                 0xF0, 0x90, 0xF0, 0x90, 0x90,
                 0xE0, 0x90, 0xE0, 0x90, 0xE0,
                 0xF0, 0x80, 0x80, 0x80, 0xF0,
                 0xE0, 0x90, 0x90, 0x90, 0xE0,
                 0xF0, 0x80, 0xF0, 0x80, 0xF0,
                 0xF0, 0x80, 0xF0, 0x80, 0x80]

    # def __init__(self):
    #     self.fname = fname

    def loadRom(self, fname):
        f = open(fname, "rb").read()
        if not f:
            return 1
        if os.stat(fname).st_size > 4096-512:
            print("ERROR: File size too big")
            f.close()
            return 1
        for i, v in enumerate(f):
            __memory[512 + i] = v
        f.close()
    
    def reset(self):
        self.__pc = 0x200
        self.__opcode = 0
        self.__i = 0
        self.__sp = 0
        self.__delayTimer = 0
        self.__soundTimer = 0
        self.drawFlag = True
        for i in range(2048):
            self.gfx[i]=0
        for i in range(4096):
            self.__memory[i] = 0
        for i in range(16):
            self.__v[i] = 0
            self.__stack[i] = 0
            self.key[i] = 0
        for i in range(80):
            self.__memory[i] = self.__fontset[i]


vm = Chip8
vm.reset(vm)
