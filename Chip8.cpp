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
//#include <memory>
const int PIXEL_SIZE = 16;

class chip8
{
public:
    chip8();
    ~chip8();
    uint8_t gfx[64 * 32];
    uint8_t key[16];
    bool drawFlag;
    bool loadRom(const char *fname);
    void reset();

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
    // empty
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
    pc = 0x200;
    opcode = 0;
    i = 0;
    sp = 0;
    delayTimer = 0;
    soundTimer = 0;
    drawFlag = true;

    for (int i = 0; i < 2048; ++i)
        gfx[i] = 0;

    for (int i = 0; i < 16; ++i)
        stack[i] = 0;
    key[i] = v[i] = 0;

    for (int i = 0; i < 4096; ++i)
        memory[i] = 0;

    for (int i = 0; i < 80; ++i)
        memory[i] = fontset[i];

    // srand(time(NULL));
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        puts("usage: chip8 <filename>");
        return 1;
    }
    chip8 vm;
    vm.reset();
    if (!vm.loadRom(argv[1]))
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
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}