#include<iostream>
#include <fstream>
#include "ds.h"



 void load_rom(){
  std::ifstream file("k.txt",std::ios::binary | std::ios::ate);
  if(file.is_open()){
   std::streampos size  = file.tellg();
   char *buffer = new char[size];
   // now the cursor is in the end of the file we need to make it at the beginning //
   file.seekg(0,std::ios::beg);
   file.read(buffer,size);// we read the content of the file into this buffer;
   file.close();
    
   for (long i=0;i<size ;i++){
      ram[0x200+i] = buffer[i];
     
   }
   delete[] buffer;

  }else{
   std::cout<<"file isnt open";
  }
 }
 int main(){
 
    std::cout<<"start of the program"<<'\n' ;
    load_rom();
    std::cout<<"finished loading success";




    while(true){
      opcode = (ram[pc] << 8u) | ram[pc + 1];
      pc+=2;
      

      uint8_t fopcode = (opcode & 0xF000u)>>12u;
      uint8_t lopcode = opcode & 0x000Fu;
      switch(fopcode){
         case 0x0:
            if(opcode == 0x00E0)op_00e0();
            else if(opcode ==0x00EE)op_00ee();
            break;
         case 0x1:
            op_1nnn();
            break;
         case 0x2:
            op_2nnn();
            break;
         case 0x3:
            op_3xkk();
            break;
         case 0x4:
            op_4xkk();
            break;
         case 0x5:
            op_5xy0();
            break;
         case 0x6:
            op_6xkk();
            break;
         case 0x7:
            op_7xkk();
            break;
         case 0x8:
           
           switch (lopcode)
           {
           case 0x0 : op_8xy0();
            break;
           case 0x1: op_8xy1();
            break;
           case 0x2: op_8xy2();
            break;
           case 0x3 : op_8xy3();
            break;
           case 0x4: op_8xy4();
            break;
           case 0x5: op_8xy5();
            break;
           case 0x6: op_8xy6();
            break;
           case 0x7: op_8xy7();
            break;

           case 0xE: op_8xyE();
            break;
           
 
           
           
           }

           break;
           case 0x9: op_9xy0();
            break;
           case 0xA :op_Annn(); break;
           case 0xB :op_Bnnn(); break;
           case 0xC :op_Cxkk();break;
           case 0xD :op_Dxyn();break;
           case 0xE :
             
             switch (lopcode)
             {
             case 0xE:
               op_Ex9E();
               break;
             case 0x1:
               op_ExA1();
               break;
             
             
             default:
               break;
             }
             break;
            case 0xF:
             
             uint16_t bytt = opcode & 0x00FFu;
             if(lopcode == 0xA) op_Fx0A();
             if(lopcode == 0xE) op_Fx1E();

             switch (bytt)
             {
             case 0x07: op_Fx07(); break;
             case 0x15: op_Fx15(); break;
             case 0x29: op_Fx29(); break;
             case 0x33: op_Fx33(); break;
             case 0x55: op_Fx55(); break;
             case 0x65: op_Fx65(); break;
             case 0x18: op_Fx18(); break;

             
             default:
               break;
             }
         
      }
      


    }
    return 0;
 }