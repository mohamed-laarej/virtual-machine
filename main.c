#include <stdio.h>
#include <string.h>
#include "vm_32b.h"

int main() {
    VM vm;
    init_VM32(&vm);

WORD program[] = {
    // Valid data access (ds_base + 0 = 0x8000)
    MOV_IMM(0, 123),
    STORE(0, 0x0),      // Store at ds_base + 0 (valid)

    // Invalid data access (ds_base + 0x4000 = 0xC000)
    MOV_IMM(1, 456),
    STORE(1, 0x4000),   // Store at 0xC000 (stack segment)

    HALT
};


    load_program32(&vm, program, sizeof(program)/sizeof(WORD));
    run_programs32(&vm);
    return 0;
}
