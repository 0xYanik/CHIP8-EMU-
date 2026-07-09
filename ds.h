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
    uint8_t Vx = opcode & 0x0F00u >> 8u; //shift the 8bits of kk to the right and make it 0x0x
    uint16_t kk = opcode & 0x00FFu ;
    if(reg[Vx] == kk) pc+=2;
}
void op_4xkk(){
    
     uint8_t Vx = opcode & 0x0F00u >> 8u; //shift the 8bits of kk to the right and make it 0x0x
    uint16_t kk = opcode & 0x00FFu ;
    if(reg[Vx] != kk) pc+=2;
}
void op_5xy0(){
    uint8_t Vx = opcode & 0x0F00u >> 8u;
    uint8_t Vy = opcode & 0X00F0u >> 4u;

    if(reg[Vx]==reg[Vy]) pc+=2;
}

void op_6xkk(){
    uint8_t Vx = opcode & 0x0F00u >> 8u;
    uint16_t kk = opcode & 0x00FFu;
    
    reg[Vx] = kk;

}

void op_7xkk(){
    uint8_t Vx = opcode & 0x0F00u >> 8u;
    uint16_t kk = opcode & 0x00FFu;
    reg[Vx]+=kk;
}
void op_8xy0(){
     uint8_t Vx = opcode & 0x0F00u >> 8u;
     uint8_t Vy = opcode & 0X00F0u >> 4u;

     reg[Vx] = reg[Vy];

}
void op_8xy1(){
     uint8_t Vx = opcode & 0x0F00u >> 8u;
     uint8_t Vy = opcode & 0X00F0u >> 4u;

     reg[Vx] = reg[Vx] | reg[Vy] ;
}
void op_8xy2(){
     uint8_t Vx = opcode & 0x0F00u >> 8u;
     uint8_t Vy = opcode & 0X00F0u >> 4u;

     reg[Vx] = reg[Vx] & reg[Vy] ;

}

void op_8xy3(){
     uint8_t Vx = opcode & 0x0F00u >> 8u;
     uint8_t Vy = opcode & 0X00F0u >> 4u;

     reg[Vx] = reg[Vx] ^ reg[Vy] ;
     
}

void op_8xy4(){
     uint8_t Vx = opcode & 0x0F00u >> 8u;
     uint8_t Vy = opcode & 0X00F0u >> 4u;

     uint16_t sum = reg[Vx]+reg[Vy];
     if(sum >255) reg[0xF] = 1;
     else reg[0xF] = 0;

     reg[Vx] = (sum)& 0xFFu; // storing only first 8bits 

     
     
}

void op_8xy5(){
     uint8_t Vx = opcode & 0x0F00u >> 8u;
     uint8_t Vy = opcode & 0X00F0u >> 4u;

     if (reg[Vx] > reg[Vy]){
        reg[0xF]=1;
     }else{
        reg[0xF] = 0;
     }

     reg[Vx]-=reg[Vy];
}

void op_8zy6(){
     uint8_t Vx = opcode & 0x0F00u >> 8u;
     uint8_t Vy = opcode & 0X00F0u >> 4u;

     uint8_t lsb = Vx & 1; // to know the value of the l significant bit 
     if (lsb ==1) reg[0xF] = 1;
     else reg[0xF] = 0;

     reg[Vx]/=2;


}

void op_8xy7(){
     uint8_t Vx = opcode & 0x0F00u >> 8u;
     uint8_t Vy = opcode & 0X00F0u >> 4u;

     if(reg[Vy]>reg[Vy]){
        reg[0xF] = 1;

     }else reg[0xF] = 0;

     reg[Vx] = reg[Vy] - reg[Vx];
}

void op_8xyE(){
     uint8_t Vx = opcode & 0x0F00u >> 8u;
     uint8_t Vy = opcode & 0X00F0u >> 4u;

     uint8_t msb = Vx & 0x8;;
     if (msb == 1) reg[0xF] = 1;
     else reg[0xF] = 0;

     reg[Vx]*=2;
}

void op_9xy0(){
     uint8_t Vx = opcode & 0x0F00u >> 8u;
     uint8_t Vy = opcode & 0X00F0u >> 4u;
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
 uint8_t Vx = opcode & 0x0F00u >> 8u;

 reg[Vx] = kk & random_num;

}

void op_Dxyn(){
    // i didnt understand shit lets comback to at later hhh
  /*  uint8_t Vx = opcode & 0x0F00u >> 8u;
     uint8_t Vy = opcode & 0X00F0u >> 4u;
    uint8_t nBytes  = (opcode & 0x000Fu);
    for (uint16_t i  = reg_index; i<=reg_index+nBytes*8 ;i++){
        display[reg[Vx]+(reg[Vy]*64)] 
    } */
}

void op_Ex9E(){
    uint8_t Vx = opcode & 0x0F00u >> 8u;
    if (keypad[reg[Vx]]){
        pc += 2;
    }
}

void op_ExA1(){
    uint8_t Vx = opcode & 0x0F00u >> 8u;
    if(keypad[reg[Vx]]==0){
        pc+=2;
    }
}


void op_Fx07(){
    uint8_t Vx = opcode & 0x0F00u >> 8u;
    reg[Vx] = delayT;
}


void op_Fx0A(){
    bool keyP  = false;
    uint8_t Vx = opcode & 0x0F00u >> 8u;
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
    uint8_t Vx = opcode & 0x0F00u >> 8u;
    delayT = Vx;
}

void op_Fx18(){
    uint8_t Vx = opcode & 0x0F00u >> 8u;
    soundT = Vx;
}

void op_Fx1E(){
     uint8_t Vx = opcode & 0x0F00u >> 8u;
     reg_index+=Vx;
}

void op_Fx29(){
    
}

#endif