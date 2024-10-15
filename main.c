#include <stdio.h>
#include <stdlib.h>

typedef unsigned char Byte;
typedef unsigned short Word;
typedef unsigned int u32;

#define MAX_MEM (1024 * 64)

struct Mem {
  Byte Data[MAX_MEM];
};

struct CPU {
  Word PC; // program counter
  Word S;  // stack pointer

  Byte A; // accumulator
  Byte X; // index register x
  Byte Y; // index register y

  Byte C : 1; // carry flag
  Byte Z : 1; // zero
  Byte I : 1; // interrupt
  Byte D : 1; // decimal
  Byte B : 1; // break
  Byte V : 1; // overflow
  Byte N : 1; // negative
};

void Initialise(struct Mem *memory) {
  for (u32 i = 0; i < 65536; i++) {
    memory->Data[i] = 0;
  }
}

void Reset(struct CPU *cpu, struct Mem memory) {
  cpu->PC = 0xFFFC;
  cpu->S = 0x0100;
  cpu->A = cpu->X = cpu->Y = 0;
  cpu->C = cpu->Z = cpu->I = cpu->D = cpu->B = cpu->V = cpu->N = 0;
  Initialise(&memory);
}

void Execute(u32 Cycles, struct CPU cpu, struct Mem memory) {
  printf("Program counter: %X\n", cpu.PC);
  printf("Stack pointer: %X\n", cpu.S);

  /*for (u32 i = 0; i < MAX_MEM; i++) {
    printf("%X", memory.Data[i]);
  } */
}
int main() {

  struct CPU cpu;
  struct Mem *mem =
      malloc(sizeof(struct Mem)); // allocate memory for the struct
  Reset(&cpu, *mem);
  Execute(1, cpu, *mem);

  /*for (u32 i = 0; i < MAX_MEM; i++) {
    printf("%X", mem->Data[i]);
  }*/

  return 0;
}
