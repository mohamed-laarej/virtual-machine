#include <stdio.h>
#include "vm_32b.h"

int validate_code_access(VM *vm, uint32_t addr) {
    if (addr < vm->cpu.cs_base || addr >= vm->cpu.cs_base + vm->cpu.cs_limit) {
        printf("Code Segment Fault: 0x%x\n", addr);
        return 0;
    }
    return 1;
}

int validate_data_access(VM *vm, uint32_t addr, int is_write) {
    if (addr < vm->cpu.ds_base || addr >= vm->cpu.ds_base + vm->cpu.ds_limit) {
        printf("Data Segment Fault: 0x%x\n", addr);
        return 0;
    }
    if (is_write && !(vm->cpu.ds_flags & 0x2)) {
        printf("Data Write Permission Denied\n");
        return 0;
    }
    return 1;
}

int validate_stack_access(VM *vm, uint32_t addr, int is_write) {
    if (addr < vm->cpu.ss_base || addr >= vm->cpu.ss_base + vm->cpu.ss_limit) {
        printf("Stack Segment Fault: 0x%x\n", addr);
        return 0;
    }
    if (is_write && !(vm->cpu.ss_flags & 0x2)) {
        printf("Stack Write Permission Denied\n");
        return 0;
    }
    return 1;
}
void init_VM32(VM *vm) {

    // Initialize registers
    for (int i = 0; i < 8; i++) {
        vm->cpu.regs[i].value = 0;
    }

    //NEW: Segment Setup
    // Code Segment: Base 0x0000, 32KB
    vm->cpu.cs_base = 0x0000;
    vm->cpu.cs_limit = 0x8000;
    vm->cpu.cs_flags = 0x5;  // 0b101 (Read + Execute)

    // Data Segment: Base 0x8000, 16KB
    vm->cpu.ds_base = 0x8000;
    vm->cpu.ds_limit = 0x4000;
    vm->cpu.ds_flags = 0x3;  // 0b011 (Read + Write)

    // Stack Segment: Base 0xC000, 32KB
    vm->cpu.ss_base = 0xC000;
    vm->cpu.ss_limit = 0x8000;
    vm->cpu.ss_flags = 0x3;  // 0b011 (Read + Write)

    // Initialize instruction pointer and stack pointer
    vm->cpu.ip = &vm->memory[vm->cpu.cs_base];
    vm->cpu.sp = vm->cpu.ss_base + vm->cpu.ss_limit - 1;
    vm->cpu.zero_flag = 0;
    vm->cpu.skip_increment = 0;



    vm->instruction_table[Add] = add_execution;
    vm->instruction_table[Sub] = sub_execution;
    vm->instruction_table[Mult] = mult_execution;
    vm->instruction_table[MovI] = movI_execution;
    vm->instruction_table[Div] = div_execution;
    vm->instruction_table[Halt] = halt_execution;
    vm->instruction_table[MovM] = movM_execution;
    vm->instruction_table[Store] = store_execution;
    vm->instruction_table[Push] = push_execution;
    vm->instruction_table[Pop] = pop_execution;
    vm->instruction_table[And] = and_execution;
    vm->instruction_table[Or] = or_execution;
    vm->instruction_table[Xor] = xor_execution;
    vm->instruction_table[Cmp] = cmp_execution;
    vm->instruction_table[Jmp] = jmp_execution;
    vm->instruction_table[Jz] = jz_execution;
    vm->instruction_table[Loads] = loads_execution;
    vm->instruction_table[Stors] = stors_execution;
    vm->instruction_table[Strcpy] = strcpy_execution;
    vm->instruction_table[Strcmp] = strcmp_execution;
    vm->instruction_table[Strcat] = strcat_execution;
    vm->instruction_table[Strlen] = strlen_execution;
    vm->instruction_table[Loadf] = loadf_execution;
    vm->instruction_table[Storef] = storef_execution;
    vm->instruction_table[Addf] = addf_execution;
    vm->instruction_table[Subf] = subf_execution;
    vm->instruction_table[Mulf] = multf_execution;
    vm->instruction_table[Divf] = divf_execution;
    vm->instruction_table[Itof] = itof_execution;
    vm->instruction_table[Ftoi] = ftoi_execution;
    vm->instruction_table[Call] = call_execution;
    vm->instruction_table[Ret] = ret_execution;

    vm->instruction_table[Mov] = mov_execution;

    // Printing values
    vm->instruction_table[PrintI] = printi_execution;
    vm->instruction_table[PrintS] = prints_execution;
    vm->instruction_table[PrintF] = printf_execution;

    // Bitwise operations
    vm->instruction_table[Shl] = shl_execution;
    vm->instruction_table[Shr] = shr_execution;

    // Conditional jumps
    vm->instruction_table[Je] = je_execution;
    vm->instruction_table[Jne] = jne_execution;

    // Reading values
    vm->instruction_table[ReadI] = readi_execution;
    vm->instruction_table[ReadS] = reads_execution;
    vm->instruction_table[ReadF] = readf_execution;

    // System calls
    vm->instruction_table[Syscall] = syscall_execution;
}

void load_program32(VM *vm, WORD *program, size_t program_size) {

    if (!validate_code_access(vm, vm->cpu.cs_base + program_size - 1)) {
        printf("Program too large for code segment!\n");
        return;
    }

    for (int i = 0; i < program_size; i++) {
        vm->memory[vm->cpu.cs_base + i] = program[i];
    }
}

void add_execution(VM *vm,WORD operand){
    uint8_t dest = (operand >> 23) & 0x7;  // Extract first 3-bit register
    uint8_t src1 = (operand >> 20) & 0x7; // Extract second 3-bit register
    uint8_t src2 = (operand >> 17) & 0x7; // Extract third 3-bit register
    vm->cpu.regs[dest].value = vm->cpu.regs[src1].value + vm->cpu.regs[src2].value;
    if(DEBUG){
        printf("Added R%d and R%d into R%d: New value of R%d = %d\n",
                       src2, src1,dest, dest, vm->cpu.regs[dest].value);
        printf("The value in R%d is: %d\n", dest, vm->cpu.regs[dest].value);
    }
}

void sub_execution(VM *vm,WORD operand){
    uint8_t dest = (operand >> 23) & 0x7;  // Extract first 3-bit register
    uint8_t src1 = (operand >> 20) & 0x7; // Extract second 3-bit register
    uint8_t src2 = (operand >> 17) & 0x7; // Extract third 3-bit register
    vm->cpu.regs[dest].value = vm->cpu.regs[src1].value - vm->cpu.regs[src2].value;
    if(DEBUG){
        printf("Substracted R%d and R%d into R%d: New value of R%d = %d\n",
                       src2, src1,dest, dest, vm->cpu.regs[dest].value);
        printf("The value in R%d is: %d\n", dest, vm->cpu.regs[dest].value);
    }
}
void div_execution(VM *vm, WORD operand){
    uint8_t dest = (operand >> 23) & 0x7;
    uint8_t src1 = (operand >> 20) & 0x7;
    uint8_t src2 = (operand >> 17) & 0x7;
    if(vm->cpu.regs[src2].value != 0){
        vm->cpu.regs[dest].value = vm->cpu.regs[src1].value / vm->cpu.regs[src2].value;
    }
    else{
        printf("ERR0R: DIVIZION_BY_ZERO!\n");
        exit(0);
    }
    if(DEBUG){
        printf("Div R%d by R%d into R%d: New value of R%d = %d\n",
               src1, src2,dest, dest, vm->cpu.regs[dest].value);
        printf("The value in R%d is: %d\n", dest, vm->cpu.regs[dest].value);
    }
}

void mult_execution(VM *vm,WORD operand){
    uint8_t dest = (operand >> 23) & 0x7;  // Extract first 3-bit register
    uint8_t src1 = (operand >> 20) & 0x7; // Extract second 3-bit register
    uint8_t src2 = (operand >> 17) & 0x7; // Extract third 3-bit register
    vm->cpu.regs[dest].value = vm->cpu.regs[src1].value * vm->cpu.regs[src2].value;
    if(DEBUG){
        printf("Multiplied R%d by R%d into R%d: New value of R%d = %d\n",
                       src2, src1,dest, dest, vm->cpu.regs[dest].value);
        printf("The value in R%d is: %d\n", dest, vm->cpu.regs[dest].value);
    }
}

// moving a value into a register
void movI_execution(VM * vm, WORD operand){
    uint8_t reg = (operand >> 23) & 0x7;
    uint32_t value = operand & 0x7FFFFF;
    vm->cpu.regs[reg].value = value;
    if(DEBUG){
        printf("the vlaue in R%d: %d\n",reg,vm->cpu.regs[reg].value);
    }
}
//moving a value from an addr in memory into a register
void movM_execution(VM *vm, WORD operand) {
    uint8_t reg = (operand >> 23) & 0x7;
    uint32_t addr = operand & 0x7FFFFF;

    if (!validate_data_access(vm, vm->cpu.ds_base + addr, 0)) {
        printf("Memory access violation at address 0x%x\n", addr);
        return;
    }

    vm->cpu.regs[reg].value = vm->memory[vm->cpu.ds_base + addr];

    if (DEBUG) {
        printf("MOVM: Loaded from memory[0x%x] to R%d (Value: %d)\n", addr, reg, vm->cpu.regs[reg].value);
    }
}

void halt_execution(VM *vm, WORD operand) {
    printf("\nExecution completed. Halting VM.\n");
    exit(0);
}
// storing the value of a register into memory
void store_execution(VM *vm, WORD operand){
    uint8_t reg = (operand >> 23) & 0x7;
    uint32_t addr = operand & 0x7FFFFF;
    uint32_t absolute_addr = vm->cpu.ds_base + addr;


    if (!validate_data_access(vm, absolute_addr, 1)) {
        printf("Memory access violation at address 0x%x\n", addr);
        exit(0);
    }

    vm->memory[absolute_addr] = vm->cpu.regs[reg].value;
    if(DEBUG){
        printf("Stored the value  %d at 0x%x\n", vm->cpu.regs[reg].value, addr);
    }
}

void push_execution(VM *vm, WORD operand){
    uint8_t reg = (operand >> 23) & 0x7;

    if (!validate_stack_access(vm, vm->cpu.sp, 1)) {
        printf("ERROR: STACK_OVER_FLOW");
        exit(0);
    }
    vm->cpu.sp--;
    vm->memory[vm->cpu.sp] = vm->cpu.regs[reg].value;
    if(DEBUG){
        printf("Pushed %d to stack\n", vm->cpu.regs[reg].value);
    }

}

void pop_execution(VM *vm, WORD operand){
    uint8_t reg = (operand >> 23) & 0x7;

    if (!validate_stack_access(vm, vm->cpu.sp, 0)) {
        printf("ERROR: STACK_UNDER_FLOW\n");
        exit(0);
    }

    vm->cpu.regs[reg].value = vm->memory[vm->cpu.sp];
    int value = vm->memory[vm->cpu.sp];
    vm->cpu.sp++;
    if(DEBUG){
        printf("Poped %d to R%d\n", value, reg);
    }

}
void and_execution(VM *vm, WORD operand){
    uint8_t dest = (operand >> 23) & 0x7;
    uint8_t src1 = (operand >> 20) & 0x7;
    uint8_t src2 = (operand >> 17) & 0x7;
    vm->cpu.regs[dest].value = vm->cpu.regs[src1].value & vm->cpu.regs[src2].value;
    if(DEBUG){
        printf("Bitwise AND of R%d and R%d: R%d = %d\n", src1, src2, dest, vm->cpu.regs[dest].value);
    }
}
void or_execution(VM *vm, WORD operand){
    uint8_t dest = (operand >> 23) & 0x7;
    uint8_t src1 = (operand >> 20) & 0x7;
    uint8_t src2 = (operand >> 17) & 0x7;
    vm->cpu.regs[dest].value = vm->cpu.regs[src1].value | vm->cpu.regs[src2].value;
    if(DEBUG){
        printf("Bitwise OR of R%d and R%d: R%d = %d\n", src1, src2, dest, vm->cpu.regs[dest].value);
    }
}
void xor_execution(VM *vm, WORD operand){
    uint8_t dest = (operand >> 23) & 0x7;
    uint8_t src1 = (operand >> 20) & 0x7;
    uint8_t src2 = (operand >> 17) & 0x7;
    vm->cpu.regs[dest].value = vm->cpu.regs[src1].value ^ vm->cpu.regs[src2].value;
    if(DEBUG){
        printf("Bitwise XOR of R%d and R%d: R%d = %d\n", src1, src2, dest, vm->cpu.regs[dest].value);
    }
}
void cmp_execution(VM *vm, WORD operand){
    uint8_t reg1 = (operand >> 23) & 0x7;
    uint8_t reg2 = (operand >> 20) & 0x7;
    int32_t val1 = vm->cpu.regs[reg1].value;
    int32_t val2 = vm->cpu.regs[reg2].value;
    vm->cpu.zero_flag = (val1 == val2);
    if(DEBUG){
        printf("Compared R%d and R%d: Zero flag = %d\n", reg1, reg2, vm->cpu.zero_flag);
    }
}
void jmp_execution(VM *vm, WORD operand) {
    if (!validate_code_access(vm, operand)) {
        printf("JMP Error: Invalid address 0x%x\n", operand);
        return;
    }

    vm->cpu.ip = &vm->memory[operand];  // Directly change the instruction pointer
    vm->cpu.skip_increment = 1;

    if (DEBUG) {
        printf("JMP: Jumping to address 0x%x\n", operand);
    }
}

void jz_execution(VM *vm, WORD operand) {
    if (vm->cpu.zero_flag) {
        if (!validate_code_access(vm, operand)) {
            printf("JZ Error: Invalid address 0x%x\n", operand);
            return;
        }

        vm->cpu.ip = &vm->memory[operand];  // Update IP to new address
        vm->cpu.skip_increment = 1;

        if (DEBUG) {
            printf("JZ: Zero flag is set. Jumping to 0x%x\n", operand);
        }
    }
}
// loading a string from memory to a register
void loads_execution(VM *vm, WORD operand){
    uint8_t reg = (operand >> 23) & 0x7;
    uint32_t addr = operand & 0x7FFFFF;
    if (!validate_data_access(vm, vm->cpu.ds_base + addr, 0)) {
        printf("Invalid address access 0x%x\n", addr);
        exit(0);
    }
    vm->cpu.regs[reg].value = vm->cpu.ds_base + addr ; // Store as byte
    if(DEBUG){
        printf("loaded from memory 0x%x to R%d\n",addr, reg);
    }
}
//storing a string from register into memory
void stors_execution(VM *vm, WORD operand){
    uint8_t reg = (operand >> 23) & 0x7;
    uint32_t addr = operand & 0x7FFFFF;
    uint32_t src = vm->cpu.regs[reg].value;
    uint32_t dest = vm->cpu.ds_base + addr;
    if (!validate_data_access(vm, src, 0) || !validate_data_access(vm, dest, 1)) {
        printf("Invalid address access 0x%x\n", addr);
        exit(0);
    }
    int i = 0;
    do{
        vm->memory[dest + i] = vm->memory[src + i];
    }while(vm->memory[src + i++] & 0xFF);
    if(DEBUG){
        printf("storing string from  R%d to memory 0x%x \n",reg, addr);
    }
}
void strlen_execution(VM *vm, WORD operand){
    uint8_t dest = (operand >> 23) & 0x7;
    uint8_t src = (operand >> 20) & 0x7;
    uint32_t addr = vm->cpu.regs[src].value;

    if (!validate_data_access(vm, addr, 0)) {
        printf("Invalid address access 0x%x\n", addr);
        exit(0);
    }
    uint32_t len = 0;
    while (vm->memory[addr + len] & 0xFF) len++;
    vm->cpu.regs[dest].value = len;
    if(DEBUG){
        printf("the length of the string is %d\n",len);
    }

}
void strcpy_execution(VM *vm, WORD operand){
    uint8_t dest_reg = (operand >> 23) & 0x7;
    uint8_t src_reg = (operand >> 20) & 0x7;
    uint32_t dest = vm->cpu.regs[dest_reg].value;
    uint32_t src = vm->cpu.regs[src_reg].value;
    if(!validate_data_access(vm, dest, 1) ||
        !validate_data_access(vm, src, 0)){
            printf("Invalid memory address ( 0x%x OR 0x%x )", src, dest);
            exit(0);
        }
    int i = 0;
    do{
        vm->memory[dest + i] = vm->memory[src + i];
    }while(vm->memory[src + i++] & 0xFF);
    if(DEBUG){
        printf("coping str from 0x%x to 0x%x\n", src, dest);
    }
}
void strcat_execution(VM *vm, WORD operand){
    uint8_t dest_reg = (operand >> 23) & 0x7;
    uint8_t src_reg = (operand >> 20) & 0x7;
    uint32_t dest_addr = vm->cpu.regs[dest_reg].value;
    uint32_t src_addr = vm->cpu.regs[src_reg].value;

    // Find the end of the destination string

    uint32_t dest_len = 0;

    while (vm->memory[dest_addr + dest_len] & 0xFF) {

    dest_len++;

    }

    // Append the source string

    uint32_t i = 0;

    while (1) {

    int32_t src_char = vm->memory[src_addr + i];

    vm->memory[dest_addr + dest_len + i] = src_char;

    if ((src_char & 0xFF) == 0) {

    break;

    }

    i++;

    }
    if(DEBUG){
     printf("Concatenated string from %d to end of %d\n", src_addr, dest_addr);
    }

}
void strcmp_execution(VM *vm, WORD operand){
    uint8_t reg1 = (operand >> 23) & 0x7;
    uint8_t reg2 = (operand >> 20) & 0x7;
    uint32_t addr1 = vm->cpu.regs[reg1].value;
    uint32_t addr2 = vm->cpu.regs[reg2].value;
    int cmp = 0;
    uint32_t i = 0;
    if(!validate_data_access(vm, addr1, 0)
       || !validate_data_access(vm, addr2, 0)){
            printf("Invalid memory address ( 0x%x OR 0x%x )", addr1, addr2);
            exit(0);
    }

    while (1) {
        int32_t c1 = vm->memory[vm->cpu.ds_base + addr1 + i] & 0xFF;
        int32_t c2 = vm->memory[vm->cpu.ds_base + addr2 + i] & 0xFF;
        if (c1 != c2) {
            cmp = (c1 > c2) ? 1 : -1;
            break;
        }
        if (!c1) break;
        i++;
    }
    vm->cpu.zero_flag = (cmp == 0);

    if(DEBUG){
        printf("cmp string in 0x%x and 0x%x result = %d\n",addr1, addr2,cmp);
    }
}
// loading a float from memory to a register
void loadf_execution(VM *vm, WORD operand) {
    uint8_t reg = (operand >> 23) & 0x7;
    uint32_t addr = operand & 0x7FFFFF;

    if (!validate_data_access(vm, vm->cpu.ds_base + addr, 0)) {
        printf("Invalid memory access at address 0x%x\n", addr);
        exit(0);
    }

    vm->cpu.regs[reg].value = vm->memory[vm->cpu.ds_base + addr];
    if (DEBUG) {
        printf("Loaded float from address 0x%x into R%d\n", addr, reg);
    }
}
//storing a float from a register into memory
void storef_execution(VM *vm, WORD operand) {
    uint8_t reg = (operand >> 23) & 0x7;
    uint32_t addr = operand & 0x7FFFFF;

    if (!validate_data_access(vm,vm->cpu.ds_base + addr, 1)) {
        printf("Invalid memory access at address 0x%x\n", addr);
        exit(0);
    }

    vm->memory[vm->cpu.ds_base + addr] = vm->cpu.regs[reg].value;
    if (DEBUG) {
        printf("Stored float from R%d into address 0x%x\n", reg, addr);
    }
}

void addf_execution(VM *vm, WORD operand) {
    uint8_t src1 = (operand >> 17) & 0x7;
    uint8_t src2 = (operand >> 20) & 0x7;
    uint8_t dest = (operand >> 23) & 0x7;
    float a, b;
    memcpy(&a, &vm->cpu.regs[src1].value, sizeof(float));
    memcpy(&b, &vm->cpu.regs[src2].value, sizeof(float));

    float result = a + b;
    memcpy(&vm->cpu.regs[dest].value, &result, sizeof(float));
    if (DEBUG) {
        printf("Added floats in R%d and R%d: Result %.2f in R%d\n", src1, src2, result, dest);
    }
}

void subf_execution(VM *vm, WORD operand) {
    uint8_t src1 = (operand >> 20) & 0x7;
    uint8_t src2 = (operand >> 17) & 0x7;
    uint8_t dest = (operand >> 23) & 0x7;
    float a, b;
    memcpy(&a, &vm->cpu.regs[src1].value, sizeof(float));
    memcpy(&b, &vm->cpu.regs[src2].value, sizeof(float));
    float result = a - b;
    memcpy(&vm->cpu.regs[dest].value, &result, sizeof(float));
    if (DEBUG) {
        printf("Subtracted floats in R%d and R%d: Result %.2f in R%d\n", src1, src2, result, dest);
    }
}

void multf_execution(VM *vm, WORD operand) {
    uint8_t src1 = (operand >> 17) & 0x7;
    uint8_t src2 = (operand >> 20) & 0x7;
    uint8_t dest = (operand >> 23) & 0x7;
    float a, b;
    memcpy(&a, &vm->cpu.regs[src1].value, sizeof(float));
    memcpy(&b, &vm->cpu.regs[src2].value, sizeof(float));
    float result = a * b;
    memcpy(&vm->cpu.regs[dest].value, &result, sizeof(float));
    if (DEBUG) {
        printf("Multiplied floats in R%d and R%d: Result %.2f in R%d\n", src1, src2, result, dest);
    }
}

void divf_execution(VM *vm, WORD operand) {
    uint8_t src1 = (operand >> 20) & 0x7;
    uint8_t src2 = (operand >> 17) & 0x7;
    uint8_t dest = (operand >> 23) & 0x7;
    float a, b;
    memcpy(&a, &vm->cpu.regs[src1].value, sizeof(float));
    memcpy(&b, &vm->cpu.regs[src2].value, sizeof(float));
    if (b == 0.0f) {
        printf("Floating point division by zero!\n");
        exit(0);
    }
    float result = a / b;
    memcpy(&vm->cpu.regs[dest].value, &result, sizeof(float));
    if (DEBUG) {
        printf("Divided floats in R%d and R%d: Result %.2f in R%d\n", src1, src2, result, dest);
    }
}

void itof_execution(VM *vm, WORD operand) {
    uint8_t dest = (operand >> 23) & 0x7;
    uint8_t src = (operand >> 20) & 0x7;
    int32_t int_val = vm->cpu.regs[src].value;
    float float_val = (float)int_val;
    memcpy(&vm->cpu.regs[dest].value, &float_val, sizeof(float));
    if (DEBUG) {
        printf("Converted integer in R%d to float: Result %.2f in R%d\n", src, float_val, dest);
    }
}

void ftoi_execution(VM *vm, WORD operand) {
    uint8_t dest = (operand >> 23) & 0x7;
    uint8_t src = (operand >> 20) & 0x7;
    float float_val;
    memcpy(&float_val, &vm->cpu.regs[src].value, sizeof(float));
    vm->cpu.regs[dest].value = (int32_t)float_val;
    if (DEBUG) {
        printf("Converted float in R%d to integer: Result %d in R%d\n", src, (int32_t)float_val, dest);
    }
}



void call_execution(VM *vm, WORD operand){
    uint32_t target_addr = operand;

    if (!validate_code_access(vm, target_addr)) {
        printf("Invalid memory access at 0x%x", target_addr);
        exit(0);
    }

    uint32_t return_addr = (uint32_t)(vm->cpu.ip - vm->memory) + 1;
    vm->cpu.sp--;

    if (!validate_stack_access(vm, vm->cpu.sp, 1)) {
        exit(0);
    }

    vm->memory[vm->cpu.sp] = return_addr;
    vm->cpu.ip = &vm->memory[target_addr];
    vm->cpu.skip_increment = 1;
    if(DEBUG){
       printf("CALL to 0x%x (return to 0x%x)\n", target_addr, return_addr);
    }
}
void ret_execution(VM *vm, WORD operand){

    uint32_t return_addr = vm->memory[vm->cpu.sp];
    vm->cpu.sp++;

    vm->cpu.ip = &vm->memory[return_addr];
    vm->cpu.skip_increment = 1;
    if(DEBUG){
        printf("RET to 0x%x\n", return_addr);
    }

}

void mov_execution(VM *vm, WORD operand){
    uint8_t reg1 = (operand >> 23) & 0x7;
    uint8_t reg2 = (operand >> 20) & 0x7;
    vm->cpu.regs[reg1].value = vm->cpu.regs[reg2].value;
    if(DEBUG){
        printf("moving the value of R%d to R%d",reg2,reg1);
    }
}
            //printing values
void printi_execution(VM *vm, WORD operand){
    uint32_t addr = operand & 0x7FFFFF;

    if (!validate_data_access(vm, vm->cpu.ds_base + addr, 0)) {
        printf("Invalid data access at 0x%x", addr);
        exit(0);
    }
    int32_t value = vm->memory[vm->cpu.ds_base + addr];
    printf("%d\n", value);
}
void prints_execution(VM *vm, WORD operand) {
    uint32_t addr = operand & 0x7FFFFF;
    char buffer[256]; // Temporary buffer
    int index = 0;

    while (index < 255) {
        char c = vm->memory[vm->cpu.ds_base + addr++];
        if (c == 0) break;  // Stop at null terminator
        buffer[index++] = c;
    }
    buffer[index] = '\0';  // Null-terminate string
    printf("%s", buffer);  // Print in one go
}

void printf_execution(VM *vm, WORD operand){
    uint32_t addr = operand & 0x7FFFFF;

    if (!validate_data_access(vm, vm->cpu.ds_base + addr, 0)) {
        printf("Invalid data access at 0x%x", addr);
        exit(0);
    }

    uint32_t float_bits = vm->memory[vm->cpu.ds_base + addr];
    float value;
    memcpy(&value, &float_bits, sizeof(float));
    printf("%.2f\n", value);
}
            //more bitwise oprations
void shl_execution(VM *vm, WORD operand){
    uint8_t dest = (operand >> 23) & 0x7;
    uint8_t src = (operand >> 20) & 0x7;
    uint8_t amt = (operand >> 17) & 0x7;
    vm->cpu.regs[dest].value = vm->cpu.regs[src].value << vm->cpu.regs[amt].value;
    if(DEBUG){
        printf("Shifted R%d left by R%d, result in R%d: %d\n",
           src, amt, dest, vm->cpu.regs[dest].value);
    }

}
void shr_execution(VM *vm, WORD operand){
    uint8_t dest = (operand >> 23) & 0x7;
    uint8_t src = (operand >> 20) & 0x7;
    uint8_t amt = (operand >> 17) & 0x7;
    vm->cpu.regs[dest].value = vm->cpu.regs[src].value >> vm->cpu.regs[amt].value;
    if(DEBUG){
        printf("Shifted R%d right by R%d, result in R%d: %d\n",
           src, amt, dest, vm->cpu.regs[dest].value);
    }
}

void je_execution(VM *vm, WORD operand){

    if(!validate_code_access(vm, operand)){
        printf("Invalid code access at 0x%x\n",operand);
        exit(0);
    }
    if (vm->cpu.zero_flag) {
        vm->cpu.ip = &vm->memory[operand];
        vm->cpu.skip_increment = 1;

        if(DEBUG){
            printf("JE: Jumping to address %d\n", operand);
        }


    }
     else { // ADD THIS ELSE BLOCK
        if (DEBUG) {
            printf("JE: No jump (ZF=%d)\n", vm->cpu.zero_flag);
        }
     }

}
void jne_execution(VM *vm, WORD operand){
    if(!validate_code_access(vm, operand)){
        printf("Invalid code access at 0x%x\n",operand);
        exit(0);
    }
    if (!vm->cpu.zero_flag) {
        vm->cpu.ip = &vm->memory[operand];
        vm->cpu.skip_increment = 1;
        if(DEBUG){
            printf("JNE: Jumping to address %d\n", operand);
        }


    }
    else{
        if(DEBUG){
            printf("JNE: No jump (ZF=%d)\n", vm->cpu.zero_flag);
        }
    }


}
            // reading a value
void readi_execution(VM *vm, WORD operand){
    uint32_t addr = operand & 0x7FFFFF;
    int32_t value;
    if(!validate_data_access(vm,vm->cpu.ds_base + addr, 1)){
        printf("Invalid data access at 0x%x", addr);
        exit(0);
    }
    scanf("%d", &value);
    vm->memory[vm->cpu.ds_base + addr] = value;
    if(DEBUG){
        printf("Stored %d at address %d\n", value, addr);
    }

}
void reads_execution(VM *vm, WORD operand){
    uint32_t addr = operand & 0x7FFFFF;
    char buffer[256];
    scanf("%255s", buffer);
    if (!validate_data_access(vm, vm->cpu.ds_base + addr, 1)) {
        printf("Invalid data access at 0x%x", addr);
        exit(0);
    }

    uint32_t current_addr = vm->cpu.ds_base + addr;
    for (int i = 0; i <= strlen(buffer); i++) {
        vm->memory[current_addr + i] = buffer[i];
    }
}
void readf_execution(VM *vm, WORD operand){
    uint32_t addr = operand & 0x7FFFFF;
    float value;
    if(!validate_data_access(vm, vm->cpu.ds_base + addr, 1)){
        printf("Invalid data access at 0x%x", addr);
        exit(0);
    }

    scanf("%f", &value);

    // Store as 32-bit IEEE 754 representation
    uint32_t float_bits;
    memcpy(&float_bits, &value, sizeof(float));
    vm->memory[vm->cpu.ds_base + addr] = float_bits;
    if(DEBUG){
        printf("Stored %.2f at address %d\n", value, addr);
    }

}
            //syscalls
void syscall_execution(VM *vm, WORD operand){
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
            break;
    }
}




void run_programs32(VM *vm) {

    while (1) {
        WORD instruction = *(vm->cpu.ip);
        uint8_t opcode = (instruction >> 26) & 0x3F;
        uint32_t operand = instruction & 0x03FFFFFF;
        vm->cpu.skip_increment = 0;
        if (vm->instruction_table[opcode]) {
            vm->instruction_table[opcode](vm, operand);  // Call function dynamically
        } else {
            printf("Unknown instruction: 0x%08X\n", instruction);
            break;
        }

        if (!vm->cpu.skip_increment) {
            vm->cpu.ip++; // Only increment if flag not set
        }
    }
}
