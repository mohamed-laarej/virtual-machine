#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "vm_16b.h"

/*
#define MOV(reg, value)  ((Mov << 12) | ((reg & 0x3) << 10) | (value & 0x3FF))
#define ADD(reg1, reg2, reg3)  ((Add << 12) | ((reg1 & 0x3) << 10) | ((reg2 & 0x3) << 8)|((reg3 & 0x3) << 6))
#define LOAD_FROM_REG(reg) ((LoadFR << 12) | ((reg & 0x3) << 10))
#define LOAD_TO_REG(reg, addr) ((LoadTR << 12) | ((reg & 0x3) << 10) | (addr & 0x3FF))*/
//******************************************************************

// Memory
/*typedef struct stack_s {
    int16_t count;
    WORD memory[MEMORY_CAPACITY];
} STACK;*/








void init_VM(VM *vm) {

    for (int i = 0; i < 4; i++) {
        vm->cpu.regs[i].id = i;
        vm->cpu.regs[i].value = 0;
    }
    vm->cpu.ip = &vm->memory[CODE_START];// Start at code section
    vm->cpu.sp = STACK_START -1; // Start execution at memory base
    vm->cpu.zero_flag = 0;
}

void load_program(VM *vm, WORD *program, size_t program_size) {
    int16_t i ;

    for (i = 0; i < program_size; i++) {
        vm->memory[i] = program[i];
    }

}

void run_prorams(VM *vm) {

    int running = 1;
    int j =0;
    while (running) {
        WORD instruction = *(vm->cpu.ip);
        uint8_t opcode = (instruction >> 12) & 0xF;
        uint16_t operand = instruction & 0x0FFF;

        switch (opcode) {
            /*case Mov: {
                uint8_t reg = (operand >> 10) & 0x3;  // Extract 2-bit register
                uint16_t value = operand & 0x3FF;    // Extract 10-bit immediate value

                vm->cpu.regs[reg].value = value;     // Store value in register

                printf("Moved %d to R%d\n", value, reg);
                break;
            }*/
            case Add: {
                uint8_t reg1 = (operand >> 10) & 0x3;  // Extract first 2-bit register
                uint8_t reg2 = (operand >> 8) & 0x3;
                uint8_t reg3= (operand >> 6) & 0x3;// Extract second 2-bit register

                // Perform addition and store in reg1
                vm->cpu.regs[reg1].value = vm->cpu.regs[reg2].value +vm->cpu.regs[reg3].value;

                printf("Added R%d and R%d: New value of R%d = %d\n",
                       reg3, reg2, reg1, vm->cpu.regs[reg1].value);
                    printf("the value in the R%d is : %d\n", vm->cpu.regs[reg1].id ,vm->cpu.regs[reg1].value);

                break;
            }
            case Sub: {
                uint8_t dest = (operand >> 10) & 0x3;
                uint8_t src1 = (operand >> 8)  & 0x3;
                uint8_t src2 = (operand >> 6)  & 0x3;
                vm->cpu.regs[dest].value = vm->cpu.regs[src1].value - vm->cpu.regs[src2].value;
                printf("Subtracted R%d from R%d: R%d = %d\n", src2, src1, dest, vm->cpu.regs[dest].value);
                break;
            }
            case Mult:{
                uint8_t reg1 = (operand >> 10) & 0x3;  // Extract first 2-bit register
                uint8_t reg2 = (operand >> 8) & 0x3;
                uint8_t reg3= (operand >> 6) & 0x3;// Extract second 2-bit register

                // Perform addition and store in reg1
                vm->cpu.regs[reg1].value = vm->cpu.regs[reg2].value * vm->cpu.regs[reg3].value;

                printf("Mult R%d and R%d: New value of R%d = %d\n",
                       reg3, reg2, reg1, vm->cpu.regs[reg1].value);
                printf("the value in the R%d is : %d\n", vm->cpu.regs[reg1].id ,vm->cpu.regs[reg1].value);
                break;
            }
            case Div:{
                uint8_t reg1 = (operand >> 10) & 0x3;  // Extract first 2-bit register
                uint8_t reg2 = (operand >> 8) & 0x3;
                uint8_t reg3= (operand >> 6) & 0x3;// Extract second 2-bit register

                // Perform addition and store in reg1
                if(vm->cpu.regs[reg3].value != 0){
                    vm->cpu.regs[reg1].value = vm->cpu.regs[reg2].value / vm->cpu.regs[reg3].value;

                printf("div R%d by R%d: New value of R%d = %d\n",
                       reg2, reg3, reg1, vm->cpu.regs[reg1].value);
                printf("the value in the R%d is : %d\n", vm->cpu.regs[reg1].id ,vm->cpu.regs[reg1].value);
                }
                else{
                    printf("DIVISION_BY_ZERO!");
                    running = 0;

                }
                break;
            }
            case MovI: {
                uint8_t reg = (operand >> 10) & 0x3;
                uint16_t value = operand & 0x3FF;
                vm->cpu.regs[reg].value = value;
                printf("Moved the value %d into R%d\n", value, reg);
                printf("the value in the R%d is : %d\n", reg ,vm->cpu.regs[reg].value);
                break;
            }
            case MovM: {
                uint8_t reg = (operand >> 10) & 0x3;
                uint16_t adrr = operand & 0x3FF;
                vm->cpu.regs[reg].value = vm->memory[adrr + DATA_START];
                printf("Moved the value in address %d into R%d\n", adrr, reg);
                printf("the value in the R%d is : %d\n", reg ,vm->cpu.regs[reg].value);


                break;
            }
            case Push: {
                uint8_t reg = (operand >> 10) & 0x3;
                vm->cpu.sp--;  // Move stack pointer down
                vm->memory[vm->cpu.sp] = vm->cpu.regs[reg].value; // Store in stack

                printf("Pushed %d from R%d to stack\n", vm->cpu.regs[reg].value, reg);
                printf("the value on the stack is %d\n", vm->memory[vm->cpu.sp]);
                break;
            }
            case Pop: {
                 // Move stack pointer up
                uint8_t reg = (operand >> 10) & 0x3;
                vm->cpu.regs[reg].value = vm->memory[vm->cpu.sp];
                vm->cpu.sp++;
                printf("Popped %d into R%d\n", vm->cpu.regs[reg].value, reg);
                printf("the value on R%d is %d\n", reg, vm->cpu.regs[reg].value);
                break;
            }

            case Store:{
                uint8_t reg = (operand >> 10) & 0x3;
                uint8_t addr = operand & 0x3FF;
                vm->memory[DATA_START + addr] = vm->cpu.regs[vm->cpu.regs[reg].id].value;
                printf("Moved the value of R%d in to adrress[%d]\n", reg, addr);
                printf("Moved the value in adrress[%d] is %d\n",addr, vm->memory[DATA_START + addr]);
                break;
            }
            case And: {
                uint8_t dest = (operand >> 10) & 0x3;
                uint8_t src1 = (operand >> 8)  & 0x3;
                uint8_t src2 = (operand >> 6)  & 0x3;
                vm->cpu.regs[dest].value = vm->cpu.regs[src1].value & vm->cpu.regs[src2].value;
                printf("Bitwise AND of R%d and R%d: R%d = %d\n", src1, src2, dest, vm->cpu.regs[dest].value);
                break;
            }
            case Or: {
                uint8_t dest = (operand >> 10) & 0x3;
                uint8_t src1 = (operand >> 8)  & 0x3;
                uint8_t src2 = (operand >> 6)  & 0x3;
                vm->cpu.regs[dest].value = vm->cpu.regs[src1].value | vm->cpu.regs[src2].value;
                printf("Bitwise OR of R%d and R%d: R%d = %d\n", src1, src2, dest, vm->cpu.regs[dest].value);
                break;
            }
            case Xor: {
                uint8_t dest = (operand >> 10) & 0x3;
                uint8_t src1 = (operand >> 8)  & 0x3;
                uint8_t src2 = (operand >> 6)  & 0x3;
                vm->cpu.regs[dest].value = vm->cpu.regs[src1].value ^ vm->cpu.regs[src2].value;
                printf("Bitwise XOR of R%d and R%d: R%d = %d\n", src1, src2, dest, vm->cpu.regs[dest].value);
                break;
            }
            case Cmp: {
                // Compare two registers and set the zero flag if equal.
                uint8_t reg1 = (operand >> 10) & 0x3;
                uint8_t reg2 = (operand >> 8)  & 0x3;
                int val1 = vm->cpu.regs[reg1].value;
                int val2 = vm->cpu.regs[reg2].value;
                vm->cpu.zero_flag = (val1 == val2);
                printf("Compared R%d and R%d: Zero flag = %d\n", reg1, reg2, vm->cpu.zero_flag);
                break;
            }
            case Jmp: {
                // Unconditional jump to the address specified in operand.
                vm->cpu.ip = &vm->memory[operand];
                printf("Jumping to address %d\n", operand);
                continue; // Skip the automatic ip increment
            }
            case Jz: {
                // Jump to address if the zero flag is set.
                if (vm->cpu.zero_flag) {
                    vm->cpu.ip = &vm->memory[operand];
                    printf("Zero flag set. Jumping to address %d\n", operand);
                    continue;
                } else {
                    printf("Zero flag not set. No jump.\n");
                }
                break;
            }
            case Halt:
                printf("Execution completed.\n");
                running = 0;
                break;
            default:
                printf("Unknown instruction: 0x%04X\n", instruction);
                running = 0;
                break;
        }
        vm->cpu.ip++;

   // Move to next instruction
    }

}



