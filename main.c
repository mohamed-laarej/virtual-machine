#include <stdio.h>
#include <string.h>
#include "vm_32b.h"


int main() {
    VM vm;
    init_VM32(&vm); // Initialize the VM
        // Initialize data segment strings
const char* prompt = "Enter number: ";
const char* result_str = "Result: ";

// Load prompt string at ds_base+0x0000
for (int i = 0; i < strlen(prompt)+1; i++) {
    vm.memory[vm.cpu.ds_base + i] = prompt[i];
}

// Load result string at ds_base+0x0020
for (int i = 0; i < strlen(result_str)+1; i++) {
    vm.memory[vm.cpu.ds_base + 0x20 + i] = result_str[i];
}

WORD fib_program[] = {
    /* Address 0x00: Main Program */
    PRINTS(0x0000),     // "Enter number: "
    READI(0x0100),      // Store input at ds_base+0x100
    MOV_MEM(0, 0x0100), // Load input to R0
    CALL(0x07),         // Call Fibonacci function
      // Store result
    PRINTS(0x0020),     // "Result: "
    PRINTI(0x0104),     // Print result
    HALT,               // [Address 0x07]
    MOV_IMM(1, 2),
    MULT(2, 0, 1),
    STORE(2, 0x0104),
    RET


};


    // Load the program into VM memory
    load_program32(&vm, fib_program, sizeof(fib_program) / sizeof(WORD));

    // Run the program
    run_programs32(&vm);

    return 0;
}


