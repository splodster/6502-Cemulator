#include <stdio.h>
#include <stdlib.h>

typedef unsigned char Byte;
typedef unsigned short Word;
typedef unsigned int u32;

#define MAX_MEM (1024 * 64) // 64 kb

#define INS_JSR_A 0x20   // JUMP TO SUBROUTINE ABSOLUTE
#define INS_LDA_IM 0xA9  // LOAD ACCUMULATOR IMMEDIATE
#define INS_LDA_ZP 0xA5  // ZERO PAGE
#define INS_LDA_ZPX 0xB5 // ZEROPAGE X

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

Word FetchW(u32 *Cycles, struct CPU *cpu, struct Mem memory) {
  Word Data = memory.Data[cpu->PC];
  cpu->PC++;
  Data |= (memory.Data[cpu->PC] << 8);
  cpu->PC++;
  (*Cycles) -= 2;
  return Data;
}

Byte Read(Byte Address, u32 *Cycles, struct CPU cpu, struct Mem memory) {
  Byte Data = memory.Data[Address];
  (*Cycles)--;
  return Data;
}

void WriteW(u32 *Cycles, struct Mem *memory, Word Value, u32 Address) {
  memory->Data[Address] = Value & 0xFF;
  memory->Data[Address + 1] = (Value >> 8);
  (*Cycles) -= 2;
}

void SetLDAStatusFlag(struct CPU *cpu) {
  cpu->Z = (cpu->A == 0);
  cpu->N = (cpu->A & 0b10000000) > 0;
}

void Execute(u32 Cycles, struct CPU *cpu, struct Mem memory) {
  while (Cycles > 0) {
    Byte Instruction = Fetch(&Cycles, cpu, memory);
    switch (Instruction) {
    case INS_LDA_IM: {
      Byte Value = Fetch(&Cycles, cpu, memory);
      cpu->A = Value;
      SetLDAStatusFlag(cpu);
    } break;
    case INS_LDA_ZP: {
      Byte ZeroPageAddress = Fetch(&Cycles, cpu, memory);
      cpu->A = Read(ZeroPageAddress, &Cycles, *cpu, memory);
      SetLDAStatusFlag(cpu);
    } break;
    case INS_LDA_ZPX: {
      Byte ZeroPageAddress = Fetch(&Cycles, cpu, memory);
      Cycles--;
      ZeroPageAddress += cpu->X;
      cpu->A = Read(ZeroPageAddress, &Cycles, *cpu, memory);
      SetLDAStatusFlag(cpu);
    } break;
    case INS_JSR_A: {
      Word SubroutineAddress = FetchW(&Cycles, cpu, memory);
      WriteW(&Cycles, &memory, (cpu->PC - 1), cpu->S);
      cpu->PC = SubroutineAddress;
      Cycles--;
    } break;
    default: {
      printf("instruction not handled %X\n", cpu->PC);
    } break;
    }
  }
}

int main() {

  struct CPU cpu;
  struct Mem *mem =
      malloc(sizeof(struct Mem)); // allocate memory for the struct
  Reset(&cpu, *mem);

  mem->Data[0xFFFC] = INS_JSR_A;
  mem->Data[0xFFFD] = 0x42;
  mem->Data[0xFFFE] = 0x42;
  mem->Data[0x4242] = INS_LDA_IM;
  mem->Data[0x4243] = 0x84;

  Execute(8, &cpu, *mem);

  printf("%X", cpu.A);

  return 0;
}
