#ifndef VM_32B_H_INCLUDED
#define VM_32B_H_INCLUDED
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// <00000000 0> 0000000 00000000 00000000
// 2^6 = 64
// 32 - 6 = 26
// 2^3 = 8
// 26 - 3 = 23

#define MEMORY_SIZE 100000

// Code segment
#define CODE_START 0x0000
#define CODE_MAX_SIZE 0x8000   // Max code size (32KB)
// Data segment
#define DATA_START CODE_START + CODE_MAX_SIZE
#define DATA_SIZE 0x4000 // 16KB
//Stack segment
#define STACK_START DATA_START + DATA_SIZE  // Stack Starts after data
#define STACK_SIZE 0x8000 //32KB
#define STACK_BOTTOM STACK_START + STACK_SIZE   // Stack end


#define REG int32_t
#define WORD int32_t



#define MOV_IMM(reg, value)  ((MovI << 26) | ((reg & 0x7) << 23) | (value & 0x7FFFFF))
#define MOV_MEM(reg, addr)   ((MovM << 26) | ((reg & 0x7) << 23) | (addr & 0x7FFFFF))
#define ADD(dest, src1, src2) ((Add << 26) | ((dest & 0x7) << 23) | ((src1 & 0x7) << 20) | (src2 & 0x7) << 17)
#define STORE(reg, addr)      ((Store << 26) | ((reg & 0x7) << 23) | (addr & 0x7FFFFF))
#define PUSH(reg)             ((Push << 26) | (reg & 0x7) << 23)
#define POP(reg)              ((Pop << 26) | (reg & 0x7) << 23)
#define MULT(dest, src1, src2) ((Mult << 26) | ((dest & 0x7) << 23) | ((src1 & 0x7) << 20) | (src2 & 0x7) << 17)
#define DIV(dest, src1, src2) ((Div << 26) | ((dest & 0x7) << 23) | ((src1 & 0x7) << 20) | (src2 & 0x7) << 17)
#define SUB(dest, src1, src2) ((Sub << 26) | ((dest & 0x7) << 23) | ((src1 & 0x7) << 20) | (src2 & 0x7) << 17)
#define AND(dest, src1, src2) ((And << 26) | (((dest & 0x7) << 23) | ((src1 & 0x7) << 20) | (src2 & 0x7) << 17)
#define OR(dest, src1, src2)  ((Or  << 26) | ((dest & 0x7) << 23) | ((src1 & 0x7) << 20) | (src2 & 0x7) << 17)
#define XOR(dest, src1, src2) ((Xor << 26) | ((dest & 0x7) << 23) | ((src1 & 0x7) << 20) | (src2 & 0x7) << 17)
#define CMP(reg1, reg2)       ((Cmp << 26) | ((reg1 & 0x7) << 23) | ((reg2 & 0x7) << 20))  // Uses only two registers
#define JMP(addr)             ((Jmp << 26) | (addr & 0x03FFFFFF))
#define JZ(addr)              ((Jz  << 26) | (addr & 0x03FFFFFF))
#define HALT                  (Halt << 26)
#define MOV(reg1, reg2)       ((Mov << 26) | ((reg1 & 0x7) << 23) | ((reg2 & 0x7) << 20))

// New String Instructions
#define LOADS(reg, addr)      ((Loads << 26) | ((reg & 0x7) << 23) | (addr & 0x7FFFFF))
#define STORS(reg, addr)      ((Stors << 26) | ((reg & 0x7) << 23) | (addr & 0x7FFFFF))
#define STRLEN(reg1, reg2)    ((Strlen << 26) | ((reg1 & 0x7) << 23) | ((reg2 & 0x7) << 20))
#define STRCPY(reg1, reg2)    ((Strcpy << 26) | ((reg1 & 0x7) << 23) | ((reg2 & 0x7) << 20))
#define STRCAT(reg1, reg2)    ((Strcat << 26) | ((reg1 & 0x7) << 23) | ((reg2 & 0x7) << 20))
#define STRCMP(reg1, reg2)    ((Strcmp << 26) | ((reg1 & 0x7) << 23) | ((reg2 & 0x7) << 20))

// New Float Instructions (Register as Address)
#define LOADF(reg, addr)      ((Loadf << 26) | ((reg & 0x7) << 23) | (addr & 0x7FFFFF))
#define STOREF(reg, addr)     ((Storef << 26) | ((reg & 0x7) << 23) | (addr & 0x7FFFFF))
#define ADDF(dest, src1, src2) ((Addf << 26) | ((dest & 0x7) << 23) | ((src1 & 0x7) << 20) | ((src2 & 0x7) << 17))
#define SUBF(dest, src1, src2) ((Subf << 26) | ((dest & 0x7) << 23) | ((src1 & 0x7) << 20) | ((src2 & 0x7) << 17))
#define MULF(dest, src1, src2) ((Mulf << 26) | ((dest & 0x7) << 23) | ((src1 & 0x7) << 20) | ((src2 & 0x7) << 17))
#define DIVF(dest, src1, src2) ((Divf << 26) | ((dest & 0x7) << 23) | ((src1 & 0x7) << 20) | ((src2 & 0x7) << 17))
#define ITOF(reg1, reg2)       ((Itof << 26) | ((reg1 & 0x7) << 23) | ((reg2 & 0x7) << 20))
#define FTOI(reg1, reg2)       ((Ftoi << 26) | ((reg1 & 0x7) << 23) | ((reg2 & 0x7) << 20))


//function calls
#define CALL(addr)   ((Call << 26) | (addr & 0x03FFFFFF))
#define RET          (Ret << 26)

// Macros for print instructions
#define PRINTI(addr)    ((PrintI << 26) | (addr & 0x7FFFFF))
#define PRINTS(addr)    ((PrintS << 26) | (addr & 0x7FFFFF))
#define PRINTF(addr)    ((PrintF << 26) | (addr & 0x7FFFFF))


// Shift Instructions
#define SHL(dest, src, amt) ( (Shl << 26) | ((dest & 0x7) << 23) | ((src & 0x7) << 20) | ((amt & 0x7) << 17) )
#define SHR(dest, src, amt) ( (Shr << 26) | ((dest & 0x7) << 23) | ((src & 0x7) << 20) | ((amt & 0x7) << 17) )

// Jump Instructions
#define JE(addr) ((Je << 26) | (addr & 0x03FFFFFF))
#define JNE(addr) ((Jne << 26) | (addr & 0x03FFFFFF))

// Read Instructions
#define READI(addr) ((ReadI << 26) | (addr & 0x7FFFFF))
#define READS(addr) ((ReadS << 26) | (addr & 0x7FFFFF))
#define READF(addr) ((ReadF << 26) | (addr & 0x7FFFFF))

// System Call
#define SYSCALL(num) ((Syscall << 26) | (num & 0x03FFFFFF))




typedef enum {
    Halt = 0x00,
    MovI = 0x01,
    MovM = 0x02,
    Add  = 0x03,
    Sub  = 0x04,
    Store= 0x05,
    Push = 0x06,
    Pop  = 0x07,
    Mult = 0x08,
    Div  = 0x09,
    And  = 0x0A,
    Or   = 0x0B,
    Xor  = 0x0C,
    Cmp  = 0x0D,
    Jmp  = 0x0E,
    Jz   = 0x0F,

    // String Instructions
    Loads = 0x10,
    Stors = 0x11,
    Strlen = 0x12,
    Strcpy = 0x13,
    Strcat = 0x14,
    Strcmp = 0x15,

    // Float Instructions
    Loadf = 0x16,
    Storef = 0x17,
    Addf = 0x18,
    Subf = 0x19,
    Mulf = 0x1A,
    Divf = 0x1B,
    Itof = 0x1C,
    Ftoi = 0x1D,
    Call = 0x1E,
    Ret  = 0x1F,
    Mov = 0x20,
    PrintI = 0x21,
    PrintS = 0x22,
    PrintF = 0x23,
    Shl = 0x24,
    Shr = 0x25,
    Je = 0x26,
    Jne = 0x27,
    ReadI = 0x28,
    ReadS = 0x29,
    ReadF = 0x2A,
    Syscall = 0x2B

} INSTRUCTION;





typedef struct {
    uint32_t value;
} REGS;
typedef struct cpu_s {
    REGS regs[8];  // General-purpose registers (R0-R3)
    WORD sp;       // Stack pointer (SP)
    WORD *ip;      // Instruction pointer
    int zero_flag;
} CPU;
typedef struct vm_s {
    CPU cpu;
    WORD memory[MEMORY_SIZE];
} VM;

/*static inline uint8_t get_reg1(WORD instruction) { return (instruction >> 23) & 0x7; }
static inline uint8_t get_reg2(WORD instruction) { return (instruction >> 20) & 0x7; }
static inline uint8_t get_reg3(WORD instruction) { return (instruction >> 17) & 0x7; }
static inline uint32_t get_address(WORD instruction) { return instruction & 0x03FFFFFF; }
static inline uint32_t get_immediate(WORD instruction) { return instruction & 0x7FFFFF; }
*/
void init_VM32(VM *vm) ;
void load_program32(VM *vm, WORD *program, size_t program_size);
void run_programs32(VM *vm);

#endif // VM_32B_H_INCLUDED
