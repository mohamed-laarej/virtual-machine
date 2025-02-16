#include <stdio.h>
#include "vm_32b.h"



int is_valid_data_address(uint32_t addr) {
    return (addr >= 0 && addr < DATA_SIZE);
}

int is_valid_code_address(uint32_t addr) {
    return (addr >= CODE_START && addr < CODE_START + CODE_MAX_SIZE);
}

int is_valid_stack_address(uint32_t addr) {
    return (addr >= STACK_START && addr < STACK_BOTTOM);
}
void init_VM32(VM *vm) {

    for (int i = 0; i < 8; i++) {
        vm->cpu.regs[i].value = 0;
    }
    vm->cpu.ip = &vm->memory[CODE_START];// Start at code section
    vm->cpu.sp = STACK_BOTTOM -1; // Start execution at memory base
    vm->cpu.zero_flag = 0;
}

void load_program32(VM *vm, WORD *program, size_t program_size) {

    for (int i = 0; i < program_size; i++) {
        vm->memory[i] = program[i];
    }

}

void run_programs32(VM *vm) {
    int running = 1;
    while (running) {
        WORD instruction = *(vm->cpu.ip);
        uint8_t opcode = (instruction >> 26) & 0x3F; // Extract 6-bit opcode
        uint32_t operand = instruction & 0x03FFFFFF; // Extract 26-bit operand

        switch (opcode) {
            case Add: {
                uint8_t reg1 = (operand >> 23) & 0x7;  // Extract first 3-bit register
                uint8_t reg2 = (operand >> 20) & 0x7; // Extract second 3-bit register
                uint8_t reg3 = (operand >> 17) & 0x7; // Extract third 3-bit register

                vm->cpu.regs[reg1].value = vm->cpu.regs[reg2].value + vm->cpu.regs[reg3].value;

                printf("Added R%d and R%d int R%: New value of R%d = %d\n",
                       reg3, reg2,reg1, reg1, vm->cpu.regs[reg1].value);
                printf("The value in R%d is: %d\n", reg1, vm->cpu.regs[reg1].value);
                break;
            }
            case Sub: {
                uint8_t dest = (operand >> 23) & 0x7;
                uint8_t src1 = (operand >> 20) & 0x7;
                uint8_t src2 = (operand >> 17) & 0x7;
                vm->cpu.regs[dest].value = vm->cpu.regs[src1].value - vm->cpu.regs[src2].value;
                printf("Subtracted R%d from R%d into R%d: R%d = %d\n", src2, src1,dest, dest, vm->cpu.regs[dest].value);
                break;
            }
            case Mult: {
                uint8_t reg1 = (operand >> 23) & 0x7;
                uint8_t reg2 = (operand >> 20) & 0x7;
                uint8_t reg3 = (operand >> 17) & 0x7;
                vm->cpu.regs[reg1].value = vm->cpu.regs[reg2].value * vm->cpu.regs[reg3].value;
                printf("Mult R%d and R%d into R%d: New value of R%d = %d\n",
                       reg3, reg2,reg1, reg1, vm->cpu.regs[reg1].value);
                printf("The value in R%d is: %d\n", reg1, vm->cpu.regs[reg1].value);
                break;
            }
            case Div: {
                uint8_t reg1 = (operand >> 23) & 0x7;
                uint8_t reg2 = (operand >> 20) & 0x7;
                uint8_t reg3 = (operand >> 17) & 0x7;
                if (vm->cpu.regs[reg3].value != 0) {
                    vm->cpu.regs[reg1].value = vm->cpu.regs[reg2].value / vm->cpu.regs[reg3].value;
                    printf("Div R%d by R%d into R%d: New value of R%d = %d\n",
                           reg2, reg3,reg1, reg1, vm->cpu.regs[reg1].value);
                    printf("The value in R%d is: %d\n", reg1, vm->cpu.regs[reg1].value);
                } else {
                    printf("DIVISION_BY_ZERO!\n");
                    running = 0;
                }
                break;
            }

            case MovI: {
                uint8_t reg = (operand >> 23) & 0x7;
                uint32_t value = operand & 0x7FFFFF; // 23-bit immediate value
                vm->cpu.regs[reg].value = value;
                printf("Moved the value %d into R%d\n", value, reg);
                printf("The value in R%d is: %d\n", reg, vm->cpu.regs[reg].value);
                break;
            }
            case MovM: {
                uint8_t reg = (operand >> 23) & 0x7;
                uint32_t addr = operand & 0x7FFFFF; // 23-bit address
                vm->cpu.regs[reg].value = vm->memory[addr + DATA_START];
                printf("Moved the value in address %d into R%d\n", addr, reg);
                printf("The value in R%d is: %d\n", reg, vm->cpu.regs[reg].value);
                break;
            }
            case Loadf: {
                    uint8_t reg = (operand >> 23) & 0x7;
                    uint32_t addr = operand & 0x7FFFFF;
                    vm->cpu.regs[reg].value = vm->memory[DATA_START + addr];
                    printf("Loaded float from address %d into R%d\n", addr, reg);
                    break;
                }
            case Storef: {
                    uint8_t reg = (operand >> 23) & 0x7;
                    uint32_t addr = operand & 0x7FFFFF;
                    vm->memory[DATA_START + addr] = vm->cpu.regs[reg].value;
                    printf("Stored float from R%d into address %d\n", reg, addr);
                    break;
                }
            case Addf: {
                    uint8_t src1 = (operand >> 17) & 0x7;
                    uint8_t src2 = (operand >> 20) & 0x7;
                    uint8_t dest = (operand >> 23) & 0x7;
                    float a, b;
                    memcpy(&a, &vm->cpu.regs[src1].value, sizeof(float));
                    memcpy(&b, &vm->cpu.regs[src2].value, sizeof(float));

                    // Perform addition
                    float result = a + b;

                    // Store result back to register
                    memcpy(&vm->cpu.regs[dest].value, &result, sizeof(float));

                    printf("Added floats in R%d and R%d: Result %.2f in R%d\n",
                           src1, src2, result, dest);
                    break;
                }
                // Similarly implement Subf, Mulf, Divf
            case Subf: {
                    uint8_t src1 = (operand >> 20) & 0x7;
                    uint8_t src2 = (operand >> 17) & 0x7;
                    uint8_t dest = (operand >> 23) & 0x7;
                    float a, b;
                    memcpy(&a, &vm->cpu.regs[src1].value, sizeof(float));
                    memcpy(&b, &vm->cpu.regs[src2].value, sizeof(float));
                    float result = a - b;
                    memcpy(&vm->cpu.regs[dest].value, &result, sizeof(float));
                    break;
                }
            case Mulf: {
                    uint8_t src1 = (operand >> 17) & 0x7;
                    uint8_t src2 = (operand >> 20) & 0x7;
                    uint8_t dest = (operand >> 23) & 0x7;
                    float a, b;
                    memcpy(&a, &vm->cpu.regs[src1].value, sizeof(float));
                    memcpy(&b, &vm->cpu.regs[src2].value, sizeof(float));
                    float result = a * b;
                    memcpy(&vm->cpu.regs[dest].value, &result, sizeof(float));
                    break;
                }
            case Divf: {
                    uint8_t src1 = (operand >> 20) & 0x7;
                    uint8_t src2 = (operand >> 17) & 0x7;
                    uint8_t dest = (operand >> 23) & 0x7;
                    float a, b;
                    memcpy(&a, &vm->cpu.regs[src1].value, sizeof(float));
                    memcpy(&b, &vm->cpu.regs[src2].value, sizeof(float));
                    if (b == 0.0f) {
                        printf("Floating point division by zero!\n");
                        running = 0;
                        break;
                    }
                    float result = a / b;
                    memcpy(&vm->cpu.regs[dest].value, &result, sizeof(float));
                    break;
                                }
            case Itof: {
                    uint8_t dest = (operand >> 23) & 0x7;
                    uint8_t src = (operand >> 20) & 0x7;
                    int32_t int_val = vm->cpu.regs[src].value;
                    float float_val = (float)int_val;
                    memcpy(&vm->cpu.regs[dest].value, &float_val, sizeof(float));
                    break;
                }
            case Ftoi: {
                    uint8_t dest = (operand >> 23) & 0x7;
                    uint8_t src = (operand >> 20) & 0x7;
                    float float_val;
                    memcpy(&float_val, &vm->cpu.regs[src].value, sizeof(float));
                    vm->cpu.regs[dest].value = (int32_t)float_val;
                    break;
                }
            case Push: {
                 uint8_t reg = (operand >> 23) & 0x7;
                vm->cpu.sp--;

                if (!is_valid_stack_address(vm->cpu.sp)) {
                    printf("Stack Overflow! SP: %d\n", vm->cpu.sp);
                    running = 0;
                    break;
                }

                vm->memory[vm->cpu.sp] = vm->cpu.regs[reg].value;
                printf("Pushed %d to stack\n", vm->cpu.regs[reg].value);
                break;
                        }
            case Pop: {
                uint8_t reg = (operand >> 23) & 0x7;

                if (!is_valid_stack_address(vm->cpu.sp)) {
                    printf("Stack Underflow! SP: %d\n", vm->cpu.sp);
                    running = 0;
                    break;
                }

                vm->cpu.regs[reg].value = vm->memory[vm->cpu.sp];
                vm->cpu.sp++;
                printf("Popped %d from stack\n", vm->cpu.regs[reg].value);
                break;
            }
            case Store: {
                uint8_t reg = (operand >> 23) & 0x7;
                uint32_t addr = operand & 0x7FFFFF;

                if (!is_valid_data_address(addr)) {
                    printf("Memory Error: Invalid data address %d\n", addr);
                    running = 0;
                    break;
                }

                vm->memory[DATA_START + addr] = vm->cpu.regs[reg].value;
                printf("Stored %d at address %d\n", vm->cpu.regs[reg].value, addr);
                break;
            }
            case And: {
                uint8_t dest = (operand >> 23) & 0x7;
                uint8_t src1 = (operand >> 20) & 0x7;
                uint8_t src2 = (operand >> 17) & 0x7;
                vm->cpu.regs[dest].value = vm->cpu.regs[src1].value & vm->cpu.regs[src2].value;
                printf("Bitwise AND of R%d and R%d: R%d = %d\n", src1, src2, dest, vm->cpu.regs[dest].value);
                break;
            }
            case Or: {
                uint8_t dest = (operand >> 23) & 0x7;
                uint8_t src1 = (operand >> 20) & 0x7;
                uint8_t src2 = (operand >> 17) & 0x7;
                vm->cpu.regs[dest].value = vm->cpu.regs[src1].value | vm->cpu.regs[src2].value;
                printf("Bitwise OR of R%d and R%d: R%d = %d\n", src1, src2, dest, vm->cpu.regs[dest].value);
                break;
            }
            case Xor: {
                uint8_t dest = (operand >> 23) & 0x7;
                uint8_t src1 = (operand >> 20) & 0x7;
                uint8_t src2 = (operand >> 17) & 0x7;
                vm->cpu.regs[dest].value = vm->cpu.regs[src1].value ^ vm->cpu.regs[src2].value;
                printf("Bitwise XOR of R%d and R%d: R%d = %d\n", src1, src2, dest, vm->cpu.regs[dest].value);
                break;
            }
            case Cmp: {
                uint8_t reg1 = (operand >> 23) & 0x7;
                uint8_t reg2 = (operand >> 20) & 0x7;
                int32_t val1 = vm->cpu.regs[reg1].value;
                int32_t val2 = vm->cpu.regs[reg2].value;
                vm->cpu.zero_flag = (val1 == val2);
                printf("Compared R%d and R%d: Zero flag = %d\n", reg1, reg2, vm->cpu.zero_flag);
                break;
            }
            case Jmp: {
                vm->cpu.ip = &vm->memory[operand];
                printf("Jumping to address %d\n", operand);
                continue; // Skip the automatic ip increment
            }
            case Jz: {
                if (vm->cpu.zero_flag) {
                    vm->cpu.ip = &vm->memory[operand];
                    printf("Zero flag set. Jumping to address %d\n", operand);
                    continue;
                } else {
                    printf("Zero flag not set. No jump.\n");
                }
                break;
            }
        case Loads: {
                uint8_t reg = (operand >> 23) & 0x7;
                uint32_t addr = operand & 0x7FFFFF;
                vm->cpu.regs[reg].value = vm->memory[DATA_START + addr] & 0xFF; // Store as byte
                break;
            }
        case Stors: {
                uint8_t reg = (operand >> 23) & 0x7;
                uint32_t addr = operand & 0x7FFFFF;
                vm->memory[DATA_START + addr] = vm->cpu.regs[reg].value & 0xFF;
                break;
            }
        case Strlen: {
                uint8_t dest = (operand >> 23) & 0x7;
                uint8_t src = (operand >> 20) & 0x7;
                uint32_t addr = vm->cpu.regs[src].value;
                uint32_t len = 0;
                while (vm->memory[DATA_START + addr + len] & 0xFF) len++;
                vm->cpu.regs[dest].value = len;
                break;
            }
        case Strcpy: {
                uint8_t dest_reg = (operand >> 23) & 0x7;
                uint8_t src_reg = (operand >> 20) & 0x7;
                uint32_t dest = vm->cpu.regs[dest_reg].value;
                uint32_t src = vm->cpu.regs[src_reg].value;
                uint32_t i = 0;
                do {
                    vm->memory[DATA_START + dest + i] = vm->memory[DATA_START + src + i];
                } while (vm->memory[DATA_START + src + i++] & 0xFF);
                break;
            }
// Similarly implement Strcat, Strcmp
        case Strcmp: {
                uint8_t reg1 = (operand >> 23) & 0x7;
                uint8_t reg2 = (operand >> 20) & 0x7;
                uint32_t addr1 = vm->cpu.regs[reg1].value;
                uint32_t addr2 = vm->cpu.regs[reg2].value;
                int cmp = 0;
                uint32_t i = 0;
                while (1) {
                    int32_t c1 = vm->memory[DATA_START + addr1 + i] & 0xFF;
                    int32_t c2 = vm->memory[DATA_START + addr2 + i] & 0xFF;
                    if (c1 != c2) {
                        cmp = (c1 > c2) ? 1 : -1;
                        break;
                    }
                    if (!c1) break;
                    i++;
                }
                vm->cpu.zero_flag = (cmp == 0);
                break;
            }
            case Strcat: {

                uint8_t dest_reg = (operand >> 23) & 0x7; // Holds destination address

                uint8_t src_reg = (operand >> 20) & 0x7;  // Holds source address

                uint32_t dest_addr = vm->cpu.regs[dest_reg].value;

                uint32_t src_addr = vm->cpu.regs[src_reg].value;

                // Find the end of the destination string

                uint32_t dest_len = 0;

                while (vm->memory[DATA_START + dest_addr + dest_len] & 0xFF) {

                dest_len++;

                }

                // Append the source string

                uint32_t i = 0;

                while (1) {

                int32_t src_char = vm->memory[DATA_START + src_addr + i];

                vm->memory[DATA_START + dest_addr + dest_len + i] = src_char;

                if ((src_char & 0xFF) == 0) {

                break;

                }

                i++;

                }

                printf("Concatenated string from %d to end of %d\n", src_addr, dest_addr);

                break;

}
            case Call: {
                uint32_t target_addr = operand;
                // Calculate return address (current IP position + 1)
                uint32_t return_addr = (uint32_t)(vm->cpu.ip - vm->memory) + 1;

                // Push return address onto the stack
                vm->cpu.sp--;
                vm->memory[vm->cpu.sp] = return_addr;

                // Jump to target address
                vm->cpu.ip = &vm->memory[target_addr];

                printf("CALL to 0x%x (return to 0x%x)\n", target_addr, return_addr);
                continue; // Skip IP increment
            }

            case Ret: {
                // Pop return address from stack
                uint32_t return_addr = vm->memory[vm->cpu.sp];
                vm->cpu.sp++;

                // Jump back
                vm->cpu.ip = &vm->memory[return_addr];

                printf("RET to 0x%x\n", return_addr);
                continue; // Skip IP increment
            }
            case Mov:{
                uint8_t reg1 = (operand >> 23) & 0x7;
                uint8_t reg2 = (operand >> 20) & 0x7;
                vm->cpu.regs[reg1].value = vm->cpu.regs[reg2].value;
                break;
            }
            case PrintI: {
                uint32_t addr = operand & 0x7FFFFF; // Extract 23-bit address
                int32_t value = vm->memory[DATA_START + addr];
                printf("%d\n", value);
                break;
            }

            case PrintS: {
                uint32_t addr = operand & 0x7FFFFF;
                uint32_t current_addr = DATA_START + addr;
                while (1) {
                    uint32_t word = vm->memory[current_addr];
                    char c = (char)(word & 0xFF); // Extract the least significant byte
                    if (c == 0) break; // Null terminator
                    putchar(c);
                    current_addr++;
                }
                break;
            }

            case PrintF: {
                uint32_t addr = operand & 0x7FFFFF;
                uint32_t float_bits = vm->memory[DATA_START + addr];
                float value;
                memcpy(&value, &float_bits, sizeof(float));
                printf("%.2f\n", value); // Print with 2 decimal places
                break;
            }
            // Shift Left
            case Shl: {
                uint8_t dest = (operand >> 23) & 0x7;
                uint8_t src = (operand >> 20) & 0x7;
                uint8_t amt = (operand >> 17) & 0x7;
                vm->cpu.regs[dest].value = vm->cpu.regs[src].value << vm->cpu.regs[amt].value;
                printf("Shifted R%d left by R%d, result in R%d: %d\n",
                       src, amt, dest, vm->cpu.regs[dest].value);
                break;
            }

            // Shift Right
            case Shr: {
                uint8_t dest = (operand >> 23) & 0x7;
                uint8_t src = (operand >> 20) & 0x7;
                uint8_t amt = (operand >> 17) & 0x7;
                vm->cpu.regs[dest].value = vm->cpu.regs[src].value >> vm->cpu.regs[amt].value;
                printf("Shifted R%d right by R%d, result in R%d: %d\n",
                       src, amt, dest, vm->cpu.regs[dest].value);
                break;
            }

            // Jump if Equal (JE)
            case Je: {
                if (vm->cpu.zero_flag) {
                    vm->cpu.ip = &vm->memory[operand];
                    printf("JE: Jumping to address %d\n", operand);
                    continue;
                }
                printf("JE: No jump (ZF=%d)\n", vm->cpu.zero_flag);
                break;
            }

            // Jump if Not Equal (JNE)
            case Jne: {
                if (!vm->cpu.zero_flag) {
                    vm->cpu.ip = &vm->memory[operand];
                    printf("JNE: Jumping to address %d\n", operand);
                    continue;
                }
                printf("JNE: No jump (ZF=%d)\n", vm->cpu.zero_flag);
                break;
            }

            // Read Integer
            case ReadI: {
                uint32_t addr = operand & 0x7FFFFF;
                int32_t value;
                printf("\nEnter integer for address %d: ", addr);
                scanf("%d", &value);
                vm->memory[DATA_START + addr] = value;
                printf("Stored %d at address %d\n", value, addr);
                break;
            }

            // Read String
            case ReadS: {
                uint32_t addr = operand & 0x7FFFFF;
                char buffer[256];
                printf("\nEnter string (max 255 chars): ");
                scanf("%255s", buffer);

                uint32_t current_addr = DATA_START + addr;
                for (int i = 0; i <= strlen(buffer); i++) { // Include null terminator
                    vm->memory[current_addr + i] = buffer[i];
                }
                break;
            }

            // System Call
            case Syscall: {
                uint32_t syscall_num = operand;
                switch (syscall_num) {
                    case 0: { // Read integer to R0
                        int32_t value;
                        printf("\nSYS_READI: Enter integer: ");
                        scanf("%d", &value);
                        vm->cpu.regs[0].value = value;
                        break;
                    }
                    case 1: { // Print integer from R0
                        printf("\nSYS_PRINTI: %d\n", vm->cpu.regs[0].value);
                        break;
                    }
                    default:
                        printf("Unknown syscall: %d\n", syscall_num);
                        running = 0;
                        break;
                }
                break;
            }
            case ReadF: {
                uint32_t addr = operand & 0x7FFFFF;
                float value;
                printf("Enter float for address %d: ", addr);
                scanf("%f", &value);

                // Store as 32-bit IEEE 754 representation
                uint32_t float_bits;
                memcpy(&float_bits, &value, sizeof(float));
                vm->memory[DATA_START + addr] = float_bits;

                printf("Stored %.2f at address %d\n", value, addr);
                break;
            }
            case Halt:
                printf("\nExecution completed.\n");
                running = 0;
                break;
            default:
                printf("Unknown instruction: 0x%08X\n", instruction);
                running = 0;
                break;
        }
        vm->cpu.ip++; // Move to next instruction
    }
}
