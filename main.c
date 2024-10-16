#include <stdio.h>
#include <stdlib.h>

typedef unsigned char Byte;
typedef unsigned short Word;
typedef unsigned int u32;

#define MAX_MEM (1024 * 64)

#define INS_LDA_IM 0xA9

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

Byte Fetch(u32 *Cycles, struct CPU *cpu, struct Mem memory) {
  Byte Data = memory.Data[cpu->PC];
  cpu->PC++;
  (*Cycles)--;
  return Data;
}

void Execute(u32 Cycles, struct CPU cpu, struct Mem memory) {
  while (Cycles > 0) {
    Byte Instruction = Fetch(&Cycles, &cpu, memory);
    switch (Instruction) {
    case INS_LDA_IM: {
      Byte Value = Fetch(&Cycles, &cpu, memory);
      cpu.A = Value;
      cpu.Z = (cpu.A == 0);
      cpu.N = (cpu.A & 0b10000000) > 0;
      printf("%X\n", cpu.PC);
      printf("%X\n", cpu.A);
      printf("%X\n", cpu.Z);
      printf("%X\n", cpu.N);
    } break;
    default: {
      printf("instruction not handled %X\n", cpu.PC);
    } break;
    }
  }
}

int main() {

  struct CPU cpu;
  struct Mem *mem =
      malloc(sizeof(struct Mem)); // allocate memory for the struct
  Reset(&cpu, *mem);

  mem->Data[0xFFFC] = 0xA9;
  mem->Data[0xFFFD] = 0x00; // indicate all 0 flag Z
  mem->Data[0xFFFE] = 0xA9;
  mem->Data[0xFFFF] = 0x80; // set negative flag

  Execute(4, cpu, *mem);

  return 0;
}
