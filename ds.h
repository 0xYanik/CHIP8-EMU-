#ifndef DS
#define DS


#include <cstdint>
#include <algorithm>
#include <iterator>

uint8_t ram[4096];

uint8_t reg[16];
uint16_t reg_index;
uint16_t pc;
uint8_t sp;
uint16_t stack[16];
uint8_t soundT; //8bit 
uint8_t delayT;
uint32_t display[64*32];//ecran de l'affichage 
uint8_t keypad[16]; // l'etat des 16 touches 

void ds_init()
    {
    pc  = 0x200;
    reg_index = 0;
    delayT = 0;
    soundT = 0;
    sp = 0;
 // 2. Nettoyage complet des tableaux (méthode robuste en C++)
    std::fill(std::begin(ram), std::end(ram), 0);
    std::fill(std::begin(reg), std::end(reg), 0);
    std::fill(std::begin(stack), std::end(stack), 0);
    std::fill(std::begin(display), std::end(display), 0);
    std::fill(std::begin(keypad), std::end(keypad), 0);
}






#endif