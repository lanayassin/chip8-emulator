#include "memory.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    struct memory mem = {0};
    memory_write(&mem, 0x200, 0xAB);

    uint8_t val;
    if (memory_read(&mem, 0x200, &val) == 0)
        printf("Valeur à 0x200 = 0x%02X\n", val);
    else
        printf("Erreur lecture mémoire\n");
}
