#include <stdio.h>
#include <string.h>
#include "vm_32b.h"

int main() {
    VM vm;
    init_VM32(&vm);

    // Initialize strings in data segment
WORD program[] = {
    // Test READF
    PRINTS(100),        // "Enter float: "
    READF(0),          // Read to DATA_START+0

    // Test memory protection
    MOV_IMM(0, 999),
    STORE(0, DATA_SIZE + 100), // Invalid data access
    HALT
};

// Initialize string at DATA_START+100
const char *prompt = "Enter float: ";
for (int i = 0; i <= strlen(prompt); i++) {
    vm.memory[DATA_START + 100 + i] = prompt[i];
}

    load_program32(&vm, program, sizeof(program)/sizeof(WORD));
    run_programs32(&vm);
    return 0;
}
