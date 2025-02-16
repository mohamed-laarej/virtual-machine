#ifndef VM_16B_H_INCLUDED
#define VM_16B_H_INCLUDED
#include <stdlib.h>
#include <stdint.h>

#define MEMORY_CAPACITY 65536
#define CODE_START 0x0000
#define CODE_SIZE  0x2000  // 8 KB code section
#define DATA_START 0x2000
#define STACK_START 0xFFFF  // Stack starts at the top
#define REG int16_t
#define WORD int16_t


#define MOV_IMM(reg, value)  ((MovI << 12) | ((reg & 0x3) << 10) | (value & 0x3FF))
#define MOV_MEM(reg, addr)   ((MovM << 12) | ((reg & 0x3) << 10) | (addr & 0x3FF))
#define ADD(dest, src1, src2) ((Add << 12) | ((dest & 0x3) << 10) | ((src1 & 0x3) << 8) | (src2 & 0x3) << 6)
#define STORE(reg, addr)      ((Store << 12) | ((reg & 0x3) << 10) | (addr & 0x3FF))
#define PUSH(reg)             ((Push << 12) | (reg & 0x3) << 10)
#define POP(reg)              ((Pop << 12) | (reg & 0x3) << 10)
#define MULT(dest, src1, src2) ((Mult << 12) | ((dest & 0x3) << 10) | ((src1 & 0x3) << 8) | (src2 & 0x3) << 6)
#define DIV(dest, src1, src2) ((Div << 12) | ((dest & 0x3) << 10) | ((src1 & 0x3) << 8) | (src2 & 0x3) << 6)
#define SUB(dest, src1, src2) ((Sub << 12) | ((dest & 0x3) << 10) | ((src1 & 0x3) << 8)  | ((src2 & 0x3) << 6))
#define AND(dest, src1, src2) ((And << 12) | ((dest & 0x3) << 10) | ((src1 & 0x3) << 8)  | ((src2 & 0x3) << 6))
#define OR(dest, src1, src2)  ((Or  << 12) | ((dest & 0x3) << 10) | ((src1 & 0x3) << 8)  | ((src2 & 0x3) << 6))
#define XOR(dest, src1, src2) ((Xor << 12) | ((dest & 0x3) << 10) | ((src1 & 0x3) << 8)  | ((src2 & 0x3) << 6))
#define CMP(reg1, reg2)       ((Cmp << 12) | ((reg1 & 0x3) << 10) | ((reg2 & 0x3) << 8))  // Uses only two registers
#define JMP(addr)             ((Jmp << 12) | (addr & 0x0FFF))
#define JZ(addr)              ((Jz  << 12) | (addr & 0x0FFF))
#define HALT                  (Halt << 12)


typedef enum {
    /*Mov = 0x01,
    Add = 0x02,
    LoadFR = 0x03,
    LoadTR = 0x04,
    Push = 0x05,
    Pop = 0x06,*/
   Halt = 0x00,
    MovI = 0x01,
    MovM = 0x02,
    Add  = 0x03,
    Store= 0x04,
    Push = 0x05,
    Pop  = 0x06,
    Mult = 0x07,
    Div  = 0x08,
    Sub  = 0x09,
    And  = 0x0A,
    Or   = 0x0B,
    Xor  = 0x0C,
    Cmp  = 0x0D,
    Jmp  = 0x0E,
    Jz   = 0x0F
} INSTRUCTION;

typedef struct {
    uint16_t id : 2;
    uint16_t value : 14;
} REGS;

typedef struct cpu_s {
    REGS regs[4];  // General-purpose registers (R0-R3)
    WORD sp;       // Stack pointer (SP)
    WORD *ip;      // Instruction pointer
    int zero_flag;
} CPU;


typedef struct vm_s {
    CPU cpu;
    WORD memory[MEMORY_CAPACITY];
} VM;

void init_VM(VM *vm);
void load_program(VM *vm, WORD *program, size_t program_size);
void run_prorams(VM *vm);

#endif // VM_16B_H_INCLUDED
