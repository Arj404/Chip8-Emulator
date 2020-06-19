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
#include <memory>
const int PIXEL_SIZE = 16;

class chip8
{
public:
    chip8();
    ~chip8();
    uint8_t frameBuffer[64 * 32];
    bool key[16];
    int last_key;
    SDL_atomic_t the_end;
    bool loadRom(const char *fname);
    void reset();
    void MainLoop();
    void SetScreen(uint8_t *buf, uint32_t w, uint32_t h);
    void executeOpcode(uint16_t opcode);

private:
    uint8_t memory[4096];
    uint8_t v[16];
    uint8_t delayTimer;
    uint8_t soundTimer;
    uint16_t stack[16];
    uint16_t I;
    uint16_t pc;
    uint16_t sp;
    uint8_t *screen;
    uint32_t screen_w, screen_h;
    static unsigned char fontset[80];
    uint16_t ArgNNN(uint16_t opcode) const;
    uint16_t ArgNN(uint16_t opcode) const;
    uint16_t ArgN(uint16_t opcode) const;
    uint16_t ArgX(uint16_t opcode) const;
    uint16_t ArgY(uint16_t opcode) const;
    void redrawScreen();
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
    pc = 0x200;
    this->I = 0;
    sp = 0;
    delayTimer = 0;
    soundTimer = 0;
    screen = nullptr;
    screen_w = 0;
    screen_h = 0;
    last_key = -1;

    for (int i = 0; i < 2048; ++i)
        frameBuffer[i] = 0x0;

    for (int i = 0; i < 16; ++i)
    {
        stack[i] = 0;
        key[i] = false;
        v[i] = 0;
    }

    for (int i = 0; i < 4096; ++i)
        memory[i] = 0;

    for (int i = 0; i < 80; ++i)
        memory[i] = fontset[i];

    srand(time(NULL));
}

void chip8::executeOpcode(uint16_t opcode)
{
    switch (opcode & 0xF000)
    {
    case 0x0000:
        switch (opcode & 0x000F)
        {
        case 0x0000: // 00E0 - CLS
            for (int i = 0; i < 2048; ++i)
                frameBuffer[i] = 0x0;
            redrawScreen();
            break;
        case 0x000E: // 00EE - RET
            --sp;
            pc = stack[sp];
            break;
        default:
            printf("Unknown opcode [0xE000]: 0x%X\n", opcode);
        }
        break;
    case 0x1000: // 1nnn - JP addr
        pc = ArgNNN(opcode);
        break;
    case 0x2000: // 2nnn - CALL addr
        stack[sp] = pc;
        ++sp;
        pc = ArgNNN(opcode);
        break;
    case 0x3000: // 3xkk - SE Vx, byte
        if (v[ArgX(opcode)] == ArgNN(opcode))
        {
            pc += 2;
        }
        break;
    case 0x4000: // 4xkk - SNE Vx, byte
        if (v[ArgX(opcode)] != ArgNN(opcode))
        {
            pc += 2;
        }
        break;
    case 0x5000: // 5xy0 - SE Vx, Vy
        if (v[ArgX(opcode)] == v[ArgY(opcode)])
        {
            pc += 2;
        }
        break;
    case 0x6000: // 6xkk - LD Vx, byte
        v[ArgX(opcode)] = ArgNN(opcode);
        break;
    case 0x7000: // 7xkk - ADD Vx, byte
        v[ArgX(opcode)] += ArgNN(opcode);
        break;
    case 0x8000:
        switch (opcode & 0x000F)
        {
        case 0x0000: // 8xy0 - LD Vx, Vy
            v[ArgX(opcode)] = v[ArgY(opcode)];
            break;
        case 0x0001: // 8xy1 - OR Vx, Vy
            v[ArgX(opcode)] |= v[ArgY(opcode)];
            break;
        case 0x0002: // 8xy2 - AND Vx, Vy
            v[ArgX(opcode)] &= v[ArgY(opcode)];
            break;
        case 0x0003: // 8xy3 - XOR Vx, Vy
            v[ArgX(opcode)] ^= v[ArgY(opcode)];
            break;
        case 0x0004: // 8xy4 - ADD Vx, Vy
            if (v[ArgY(opcode)] > (0xFF - v[ArgX(opcode)]))
            {
                v[0xF] = 1;
            }
            else
            {
                v[0xF] = 0;
            }
            v[ArgX(opcode)] += v[ArgY(opcode)];
            break;
        case 0x0005: // 8xy5 - SUB Vx, Vy
            if (v[ArgX(opcode)] > v[ArgY(opcode)])
            {
                v[0xF] = 1;
            }
            else
            {
                v[0xF] = 0;
            }
            v[ArgX(opcode)] -= v[ArgY(opcode)];
            break;
        case 0x0006: // 8xy6 - SHR Vx {, Vy}
            v[0xF] = v[ArgX(opcode)] & 0x1;
            v[ArgX(opcode)] >>= 1;
            break;
        case 0x0007: // 8xy7 - SUBN Vx, Vy
            if (v[ArgX(opcode)] > v[ArgY(opcode)])
            {
                v[0xF] = 0;
            }
            else
            {
                v[0xF] = 1;
            }
            v[ArgX(opcode)] = v[ArgY(opcode)] - v[ArgX(opcode)];
            break;
        case 0x000E: // 8xyE - SHL Vx {, Vy}
            v[0xF] = v[ArgX(opcode)] >> 7;
            v[ArgX(opcode)] <<= 1;
            break;
        default:
            printf("Unknown opcode [0xE000]: 0x%X\n", opcode);
        }
        break;
    case 0x9000: // 9xy0 - SNE Vx, Vy
        if (v[ArgX(opcode)] != v[ArgY(opcode)])
        {
            pc += 2;
        }
        break;
    case 0xA000: // Annn - LD I, addr
        I = ArgNNN(opcode);
        break;
    case 0xB000: // Bnnn - JP V0, addr
        pc = (ArgNNN(opcode) + v[0]) & 0x0FFF;
        break;
    case 0xC000: // Cxkk - RND Vx, byte
        v[ArgX(opcode)] = (rand() % 0xFF) & ArgNN(opcode);
        break;
    case 0xD000: // Dxyn - DRW Vx, Vy, nibble
                 // NOTE : wrap around in v[ArgY(opcode)] % 32 and v[ArgX(opcode)] % 64??
        for (int j = 0; j < ArgN(opcode); j++)
        {
            uint8_t px = memory[I + j];
            for (int k = 0; k < 8; k++)
            {
                if (((px >> (7 - k)) & 1) && (frameBuffer[(v[ArgX(opcode)] % 64) + k + ((v[ArgY(opcode)] % 32) + j) * 64]))
                {
                    v[0xF] = 1;
                }
                frameBuffer[(v[ArgX(opcode)] % 64) + k + ((v[ArgY(opcode)] % 32) + j) * 64] ^= (px >> (7 - k)) & 1;
            }
        }
        redrawScreen();
        break;
    case 0xE000:
        switch (opcode & 0x00FF)
        {
        case 0x009E: // Ex9E - SKP Vx
            if (key[v[ArgX(opcode)]] != 0)
                pc += 2;
            break;
        case 0x00A1: // ExA1 - SKNP Vx
            if (!key[v[ArgX(opcode)]])
                pc += 2;
            break;
        default:
            printf("Unknown opcode [0xE000]: 0x%X\n", opcode);
        }
        break;
    case 0xF000:
        switch (opcode & 0x00FF)
        {
        case 0x0007: // Fx07 - LD Vx, DT
            v[ArgX(opcode)] = delayTimer;
            break;
        case 0x000A: // Fx0A - LD Vx, K
            // NOTE check
            while (last_key == -1)
            {
                SDL_Delay(5);
            }
            v[ArgX(opcode)] = last_key;
            break;
        case 0x0015: // Fx15 - LD DT, Vx
            delayTimer = v[ArgX(opcode)];
            break;
        case 0x0018: // Fx18 - LD ST, Vx
            soundTimer = v[ArgX(opcode)];
            break;
        case 0x001E: // Fx1E - ADD I, Vx
            if (I + v[ArgX(opcode)] > 0x0FFF)
                v[0xF] = 1;
            else
                v[0xF] = 0;
            I = (I + v[ArgX(opcode)]) & 0x0FFF;
            break;
        case 0x0029: // Fx29 - LD F, Vx
            // NOTE check
            I = v[ArgX(opcode)] * 0x5;
            break;
        case 0x0033: // Fx33 - LD B, Vx
            memory[I + 0] = v[ArgX(opcode)] / 100;
            memory[I + 1] = (v[ArgX(opcode)] / 10) % 10;
            memory[I + 2] = (v[ArgX(opcode)] % 100) % 10;
            break;
        case 0x0055: // Fx55 - LD [I], Vx
            for (int i = 0; i <= ArgX(opcode); ++i)
                memory[I + i] = v[i];
            I += ArgX(opcode) + 1;
            break;
        case 0x0065: // Fx65 - LD Vx, [I]
            for (int i = 0; i <= ArgX(opcode); ++i)
                v[i] = memory[I + i];
            I += ArgX(opcode) + 1;
            break;
        default:
            printf("Unknown opcode [0xE000]: 0x%X\n", opcode);
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

void chip8::redrawScreen()
{
    if (screen == nullptr)
    {
        return;
    }
    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            for (int k = y * PIXEL_SIZE; k < (y + 1) * PIXEL_SIZE; k++)
            {
                for (int j = x * PIXEL_SIZE; j < (x + 1) * PIXEL_SIZE; j++)
                {
                    screen[(k * screen_w + j) * 4 + 0] = frameBuffer[x + y * 64] * 0xFF; // blue
                    screen[(k * screen_w + j) * 4 + 1] = frameBuffer[x + y * 64] * 0xFF; // green
                    screen[(k * screen_w + j) * 4 + 2] = frameBuffer[x + y * 64] * 0xFF; // red
                    screen[(k * screen_w + j) * 4 + 3] = frameBuffer[x + y * 64] * 0xFF; // opacity
                }
            }
        }
    }
}

void chip8::SetScreen(uint8_t *buf, uint32_t w, uint32_t h)
{
    screen = buf;
    screen_h = h;
    screen_w = w;
}

void chip8::MainLoop()
{
    uint32_t last_ticks = SDL_GetTicks();
    for (;;)
    {
        if (SDL_AtomicGet(&the_end) != 0)
        {
            break;
        }
        // Handle timers.
        uint32_t now = SDL_GetTicks();
        if (now - last_ticks > 16)
        {
            uint32_t diff = now - last_ticks;
            uint32_t timer_ticks = diff / 16;

            delayTimer = std::max(0, (int)delayTimer - (int)timer_ticks);
            soundTimer = std::max(0, (int)soundTimer - (int)timer_ticks);

            last_ticks = now - diff % 16;
        }
        // Execute Opcode
        uint16_t opcode;
        if (pc + 1 >= 4096)
        {
            printf("error: pc out of bound (%.4x)\n", pc);
            return;
        }

        opcode = memory[pc] << 8 | memory[pc + 1];
        pc += 2;
        executeOpcode(opcode);
        SDL_Delay(1); // comment to run at full speed
    }
}

int VMThread(void *vm_obj)
{
    chip8 *vm = (chip8 *)vm_obj;
    vm->MainLoop();
    return 0;
}

int TranslateCodeToIndex(SDL_Keycode key)
{
    switch (key)
    {
    case SDLK_1:
        return 1;
    case SDLK_2:
        return 2;
    case SDLK_3:
        return 3;
    case SDLK_4:
        return 12;
    case SDLK_q:
        return 4;
    case SDLK_w:
        return 5;
    case SDLK_e:
        return 6;
    case SDLK_r:
        return 13;
    case SDLK_a:
        return 7;
    case SDLK_s:
        return 8;
    case SDLK_d:
        return 9;
    case SDLK_f:
        return 14;
    case SDLK_z:
        return 10;
    case SDLK_x:
        return 0;
    case SDLK_c:
        return 11;
    case SDLK_v:
        return 15;
    case SDLK_DOWN:
        return 8;
    case SDLK_UP:
        return 2;
    case SDLK_LEFT:
        return 4;
    case SDLK_RIGHT:
        return 6;
    }
    return -1;
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
    vm->SetScreen((uint8_t *)surface->pixels, surface->w, surface->h);

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
            if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
            {
                int idx = TranslateCodeToIndex(e.key.keysym.sym);
                if (idx != -1)
                {
                    vm->key[idx] = e.key.state == SDL_PRESSED;

                    if (e.key.state == SDL_PRESSED)
                    {
                        vm->last_key = idx;
                    }
                    else
                    {
                        bool all_keys_are_released = true;
                        for (int i = 0; i < 16; i++)
                        {
                            if (vm->key[i])
                            {
                                all_keys_are_released = false;
                                break;
                            }
                        }

                        if (all_keys_are_released)
                        {
                            vm->last_key = -1;
                        }
                    }
                }
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