///////////////////////////////////////////////////////////////////////////////
// Name: Chip8 VM
//
// Author: Arjav Jain
// Contact: arjavjain598@gmail.com
//
// License: MIT style license
///////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <SDL2/SDL.h>
#undef main
//#include <stdint.h>
#include <memory>
const int PIXEL_SIZE = 16;

class chip8
{
public:
    chip8();
    ~chip8();
    uint8_t gfx[64 * 32];
    uint8_t key[16];
    bool drawFlag;
    SDL_atomic_t the_end;
    bool loadRom(const char *fname);
    void reset();
    void MainLoop();

private:
    uint8_t memory[4096];
    uint8_t v[16];
    uint8_t delayTimer;
    uint8_t soundTimer;
    uint16_t stack[16];
    uint16_t i;
    uint16_t pc;
    uint16_t opcode;
    uint16_t sp;
    static unsigned char fontset[80];
    void executeOpcode();
    uint16_t ArgNNN(uint16_t opcode) const;
    uint16_t ArgNN(uint16_t opcode) const;
    uint16_t ArgN(uint16_t opcode) const;
    uint16_t ArgX(uint16_t opcode) const;
    uint16_t ArgY(uint16_t opcode) const;
};

unsigned char chip8::fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

chip8::chip8()
{
    reset();
}

chip8::~chip8()
{
    // empty
}

bool chip8::loadRom(const char *fname)
{
    FILE *f = fopen(fname, "rb");
    if (f == NULL)
    {
        printf("ERROR: File error");
        return false;
    }
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    rewind(f);
    if ((4096 - 512) < fsize)
    {
        puts("ERROR: File size too large");
        fclose(f);
        return false;
    }
    long result = fread(memory + 512, 1, fsize, f);
    if (result != fsize)
    {
        puts("ERROR: File not loaded in memory");
        fclose(f);
        return false;
    }
    fclose(f);
    return true;
}

void chip8::reset()
{
    this->pc = 0x200;
    this->opcode = 0;
    this->i = 0;
    this->sp = 0;
    this->delayTimer = 0;
    this->soundTimer = 0;
    this->drawFlag = true;

    for (int i = 0; i < 2048; ++i)
        gfx[i] = 0;

    for (int i = 0; i < 16; ++i)
    {
        stack[i] = 0;
        key[i] = v[i] = 0;
    }

    for (int i = 0; i < 4096; ++i)
        memory[i] = 0;

    for (int i = 0; i < 80; ++i)
        memory[i] = fontset[i];

    // srand(time(NULL));
}

void chip8::executeOpcode()
{
    switch (opcode & 0xF000)
    {
    case 0x0000:
        switch (opcode & 0x000F)
        {
        case 0x0000: // 00E0 - CLS
            break;
        case 0x000E: // 00EE - RET
            break;
        }
        break;
    case 0x1000: // 1nnn - JP addr
        break;
    case 0x2000: // 2nnn - CALL addr
        break;
    case 0x3000: // 3xkk - SE Vx, byte
        break;
    case 0x4000: // 4xkk - SNE Vx, byte
        break;
    case 0x5000: // 5xy0 - SE Vx, Vy
        break;
    case 0x6000: // 6xkk - LD Vx, byte
        break;
    case 0x7000: // 7xkk - ADD Vx, byte
        break;
    case 0x8000:
        switch (opcode & 0x000F)
        {
        case 0x0000: // 8xy0 - LD Vx, Vy
            break;
        case 0x0001: // 8xy1 - OR Vx, Vy
            break;
        case 0x0002: // 8xy2 - AND Vx, Vy
            break;
        case 0x0003: // 8xy3 - XOR Vx, Vy
            break;
        case 0x0004: // 8xy4 - ADD Vx, Vy
            break;
        case 0x0005: // 8xy5 - SUB Vx, Vy
            break;
        case 0x0006: // 8xy6 - SHR Vx {, Vy}
            break;
        case 0x0007: // 8xy7 - SUBN Vx, Vy
            break;
        case 0x000E: // 8xyE - SHL Vx {, Vy}
            break;
        }
        break;
    case 0x9000: // 9xy0 - SNE Vx, Vy
        break;
    case 0xA000: // Annn - LD I, addr
        break;
    case 0xB000: // Bnnn - JP V0, addr
        break;
    case 0xC000: // Cxkk - RND Vx, byte
        break;
    case 0xD000: // Dxyn - DRW Vx, Vy, nibble
        break;
    case 0xE000:
        switch (opcode & 0x00FF)
        {
        case 0x009E: // Ex9E - SKP Vx
            break;
        case 0x00A1: // ExA1 - SKNP Vx
            break;
        }
        break;
    case 0xF000:
        switch (opcode & 0x00FF)
        {
        case 0x0007: // Fx07 - LD Vx, DT
            break;
        case 0x000A: // Fx0A - LD Vx, K
            break;
        case 0x0015: // Fx15 - LD DT, Vx
            break;
        case 0x0018: // Fx18 - LD ST, Vx
            break;
        case 0x001E: // Fx1E - ADD I, Vx
            break;
        case 0x0029: // Fx29 - LD F, Vx
            break;
        case 0x0033: // Fx33 - LD B, Vx
            break;
        case 0x0055: // Fx55 - LD [I], Vx
            break;
        case 0x0065: // Fx65 - LD Vx, [I]
            break;
        }
        break;
    default:
        printf("Unknown opcode: 0x%X\n", opcode);
    }
}

uint16_t chip8::ArgNNN(uint16_t opcode) const
{
    return opcode & 0x0FFF;
}

uint16_t chip8::ArgNN(uint16_t opcode) const
{
    return opcode & 0x00FF;
}

uint16_t chip8::ArgN(uint16_t opcode) const
{
    return opcode & 0x000F;
}

uint16_t chip8::ArgX(uint16_t opcode) const
{
    return (opcode & 0x0F00) >> 8;
}

uint16_t chip8::ArgY(uint16_t opcode) const
{
    return (opcode & 0x00F0) >> 4;
}

void chip8::MainLoop()
{
    for (;;)
    {
        if (SDL_AtomicGet(&the_end) != 0)
        {
            break;
        }
        SDL_Delay(1);
    }
}

int VMThread(void *vm_obj)
{
    chip8 *vm = (chip8 *)vm_obj;
    vm->MainLoop();
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        puts("usage: chip8 <filename>");
        return 1;
    }
    auto vm = std::make_unique<chip8>();
    if (!vm->loadRom(argv[1]))
    {
        puts("ERROR: Failed to read rom");
        return 2;
    }

    //display
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0)
    {
        puts("ERROR: SDL Failed");
        return 3;
    }
    SDL_Window *win = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 64 * PIXEL_SIZE, 32 * PIXEL_SIZE, SDL_WINDOW_SHOWN);
    if (win == nullptr)
    {
        puts("ERROR: Window creation failed");
        SDL_Quit();
        return 3;
    }
    // SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    // if (ren == nullptr)
    // {
    //     SDL_DestroyWindow(win);
    //     puts("ERROR: Window Rendering failed");
    //     SDL_Quit();
    //     return 1;
    // }
    SDL_Surface *surface = SDL_GetWindowSurface(win);
    SDL_Thread *th = SDL_CreateThread(VMThread, "vm thread", vm.get());
    uint8_t *px = (uint8_t *)surface->pixels;
    px[256 * surface->pitch + 512 * 4] = 0xff;

    SDL_Event e;
    bool the_end = false;
    while (!the_end)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                the_end = true;
                break;
            }
        }
        SDL_UpdateWindowSurface(win);
    }

    //SDL_DestroyRenderer(ren);
    SDL_AtomicSet(&vm->the_end, 1);
    SDL_WaitThread(th, nullptr);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}