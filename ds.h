#ifndef DS
#define DS


#include <cstdint>
#include <algorithm>
#include <iterator>
#include <random>

uint8_t ram[4096];

uint8_t reg[16];
uint16_t reg_index;
uint16_t pc;
uint8_t sp;
uint16_t opcode;
uint16_t stack[16];
uint8_t soundT; //8bit 
uint8_t delayT;
uint32_t display[64*32];//ecran de l'affichage 
uint8_t keypad[16]; // l'etat des 16 touches 
const unsigned int FONTSET_SIZE = 80;
uint8_t fontset[FONTSET_SIZE] =
{
0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
0x20, 0x60, 0x20, 0x20, 0x70, // 1
0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
0x90, 0x90, 0xF0, 0x10, 0x10, // 4
0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
0xF0, 0x10, 0x20, 0x40, 0x40, // 7
0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
0xF0, 0x90, 0xF0, 0x90, 0x90, // A
0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
0xF0, 0x80, 0x80, 0x80, 0xF0, // C
0xE0, 0x90, 0x90, 0x90, 0xE0, // D
0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
0xF0, 0x80, 0xF0, 0x80, 0x80 // F
};
void ds_init()
    {
    pc  = 0x200;
    reg_index = 0;
    delayT = 0;
    soundT = 0;
    sp = 0;
    // set 0 in all cases 
    std::fill(std::begin(ram), std::end(ram), 0);
    std::fill(std::begin(reg), std::end(reg), 0);
    std::fill(std::begin(stack), std::end(stack), 0);
    std::fill(std::begin(display), std::end(display), 0);
    std::fill(std::begin(keypad), std::end(keypad), 0);


    for (unsigned int i=0;i<80;i++){
        ram[0x50+i] = fontset[i];
    }
}

void op_00e0(){
    //instruction to clear the display
    std::fill(std::begin(display),std::end(display),0);

}

void op_00ee(){
    // return from a subroutine //
    --sp;
    pc = stack[sp];

}

void op_1nnn(){
    // jump to a specific adress nnn (a jump doesnt remember its origin)  1nnn: jp adresse
    uint16_t addr = opcode & 0x0fffu;
    pc = addr;
}

void op_2nnn(){
    // 2nn is a call    we save the adresse and we call another 
    uint16_t addr = opcode & 0x0fffu;
    stack[sp] = pc;
    ++sp;
    pc = addr;

}

void op_3xkk(){
    uint8_t Vx = (opcode & 0x0F00u )>> 8u; //shift the 8bits of kk to the right and make it 0x0x
    uint16_t kk = opcode & 0x00FFu ;
    if(reg[Vx] == kk) pc+=2;
}
void op_4xkk(){
    
     uint8_t Vx = (opcode & 0x0F00u )>> 8u; //shift the 8bits of kk to the right and make it 0x0x
    uint16_t kk = opcode & 0x00FFu ;
    if(reg[Vx] != kk) pc+=2;
}
void op_5xy0(){
      uint8_t Vx = (opcode & 0x0F00u )>> 8u;
     uint8_t Vy = (opcode & 0X00F0u )>> 4u;

    if(reg[Vx]==reg[Vy]) pc+=2;
}

void op_6xkk(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint16_t kk = opcode & 0x00FFu;
    
    reg[Vx] = kk;

}

void op_7xkk(){
    uint8_t Vx =( opcode & 0x0F00u )>> 8u;
    uint16_t kk = opcode & 0x00FFu;
    reg[Vx]+=kk;
}
void op_8xy0(){
        uint8_t Vx = (opcode & 0x0F00u )>> 8u;
     uint8_t Vy = (opcode & 0X00F0u )>> 4u;

     reg[Vx] = reg[Vy];

}
void op_8xy1(){
      uint8_t Vx = (opcode & 0x0F00u )>> 8u;
     uint8_t Vy = (opcode & 0X00F0u )>> 4u;

     reg[Vx] = reg[Vx] | reg[Vy] ;
}
void op_8xy2(){
      uint8_t Vx = (opcode & 0x0F00u )>> 8u;
     uint8_t Vy = (opcode & 0X00F0u )>> 4u;

     reg[Vx] = reg[Vx] & reg[Vy] ;

}

void op_8xy3(){
        uint8_t Vx = (opcode & 0x0F00u )>> 8u;
     uint8_t Vy = (opcode & 0X00F0u )>> 4u;

     reg[Vx] = reg[Vx] ^ reg[Vy] ;
     
}

void op_8xy4(){
      uint8_t Vx = (opcode & 0x0F00u )>> 8u;
     uint8_t Vy = (opcode & 0X00F0u )>> 4u;

     uint16_t sum = reg[Vx]+reg[Vy];
     if(sum >255) reg[0xF] = 1;
     else reg[0xF] = 0;

     reg[Vx] = (sum)& 0xFFu; // storing only first 8bits 

     
     
}

void op_8xy5(){
     uint8_t Vx = (opcode & 0x0F00u )>> 8u;
     uint8_t Vy = (opcode & 0X00F0u )>> 4u;

     if (reg[Vx] > reg[Vy]){
        reg[0xF]=1;
     }else{
        reg[0xF] = 0;
     }

     reg[Vx]-=reg[Vy];
}

void op_8xy6(){
     uint8_t Vx =( opcode & 0x0F00u) >> 8u;
     uint8_t Vy = (opcode & 0X00F0u )>> 4u;

     uint8_t lsb = reg[Vx] & 1; // to know the value of the l significant bit 
     if (lsb ==1) reg[0xF] = 1;
     else reg[0xF] = 0;

     reg[Vx]/=2;


}

void op_8xy7(){
     uint8_t Vx = (opcode & 0x0F00u) >> 8u;
     uint8_t Vy = (opcode & 0X00F0u) >> 4u;

     if(reg[Vy]>reg[Vx]){
        reg[0xF] = 1;

     }else reg[0xF] = 0;

     reg[Vx] = reg[Vy] - reg[Vx];
}

void op_8xyE(){
     uint8_t Vx = (opcode & 0x0F00u) >> 8u;
     uint8_t Vy = (opcode & 0X00F0u) >> 4u;

     uint8_t msb = (reg[Vx] & 0x80u) >> 7u;//decaler le 7 bit adroite 
     if (msb == 1) reg[0xF] = 1;
     else reg[0xF] = 0;

     reg[Vx]*=2;
}

void op_9xy0(){
     uint8_t Vx = (opcode & 0x0F00u) >> 8u;
     uint8_t Vy = (opcode & 0X00F0u )>> 4u;
    if(reg[Vx] != reg[Vy]) pc+=2;
}

void op_Annn(){
    uint16_t addr = opcode & 0x0FFF;
    reg_index = addr;
}

void op_Bnnn(){ // a jump
    uint16_t addr = opcode & 0x0FFF;
    pc = reg[0x0] + addr;

}

void op_Cxkk(){
 std::random_device rd;
 std::mt19937 gen(rd());
 std::uniform_int_distribution<int> distrib(0, 255);
 uint8_t random_num = distrib(gen);

 uint16_t kk = opcode & 0x00FFu;
 uint8_t Vx =( opcode & 0x0F00u) >> 8u;

 reg[Vx] = kk & random_num;

}

void op_Dxyn() {
    uint16_t width = 64;
    uint16_t height = 32;
    
    // Extraction des registres et de la hauteur du sprite (n)
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint8_t nbytes = (opcode & 0x000Fu);

    // Coordonnées de départ (avec gestion du wrapping/modulo selon la spec CHIP-8)
    uint8_t x_start = reg[Vx] % width;
    uint8_t y_start = reg[Vy] % height;

    // Réinitialisation du registre de collision VF
    reg[0xF] = 0;

    // Boucle sur chaque ligne du sprite (hauteur)
    for (uint8_t row = 0; row < nbytes; row++) {
        // Lecture de l'octet du sprite en RAM (l'index commence souvent à 0, pas à 1)
        uint8_t sprite_byte = ram[reg_index + row];
        
        // Coordonnée Y actuelle (si le sprite dépasse le bas, il s'arrête ou wrap selon les versions)
        uint8_t current_y = y_start + row;
        if (current_y >= height) break; // Arrêt standard si on dépasse le bas de l'écran

        // Boucle sur les 8 bits de l'octet (largeur fixe de 8 pixels pour un sprite CHIP-8)
        for (uint8_t col = 0; col < 8; col++) {
            uint8_t current_x = x_start + col;
            if (current_x >= width) break; // Arrêt standard si on dépasse la droite de l'écran

            // On vérifie si le bit actuel du sprite est allumé (1)
            // 0x80u vaut 10000000 en binaire. On décale vers la droite pour tester chaque bit.
            if ((sprite_byte & (0x80u >> col)) != 0) {
                
                // Calcul de l'index 1D de l'écran
                uint16_t screen_index = (current_y * width) + current_x;

                // Détection de collision : si le pixel à l'écran est déjà allumé (1)
                if (display[screen_index] == 1) {
                    reg[0xF] = 1; // Collision détectée !
                }

                // Application du XOR (allume ou éteint le pixel)
                display[screen_index] ^= 1; 
            }
        }
    }
}

void op_Ex9E(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    if (keypad[reg[Vx]]){
        pc += 2;
    }
}

void op_ExA1(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    if(keypad[reg[Vx]]==0){
        pc+=2;
    }
}


void op_Fx07(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    reg[Vx] = delayT;
}


void op_Fx0A(){
    bool keyP  = false;
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    for (uint8_t i = 0;i<16;i++){
        if(keypad[i]==1){
            keyP = true;
            reg[Vx] = i;//we store the value of the key inside reg

            break;
        }
    }

    if (keyP == false){
        pc-=2;//stop the auto incremntation of pc
    }


}


void op_Fx15(){
    uint8_t Vx = (opcode & 0x0F00u )>> 8u;
    delayT = reg[Vx];
}

void op_Fx18(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    soundT = reg[Vx];
}

void op_Fx1E(){
     uint8_t Vx = (opcode & 0x0F00u) >> 8u;
     reg_index+=Vx;
}

void op_Fx29(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    reg_index = 0x50 + reg[Vx] * 5;
}


void op_Fx33(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    ram[reg_index] = reg[Vx]/100;
    ram[reg_index+1] = (reg[Vx]/10)%10;
    ram[reg_index+2] = reg[Vx]%10;
}


void op_Fx55(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    for (uint8_t i = 0;i<=Vx;i++){
        ram[reg_index+i] = reg[i];
    }
}


void op_Fx65(){
    uint8_t Vx = (opcode & 0x0F00u )>> 8u;
       for (uint8_t i = 0;i<=Vx;i++){
        reg[i] = ram[reg_index+i];
    }
}





#endif