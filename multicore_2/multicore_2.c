#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

// Função que será executada no Core 0
void core0_entry() {
    for (int i = 0; i <= 20; i++) {
        printf("Rodando no Core 0 - Iteração %d\n", i);
        sleep_ms(1000);
    }
}

// Função que será executada no Core 1
void core1_entry() {
    for (int i = 0; i <= 20; i++) {
        printf("Rodando no Core 1 - Iteração %d\n", i);
        sleep_ms(1000);
    }
}

int main() {
    stdio_init_all(); // Inicializa a comunicação serial

    // Função no Core 0
    core0_entry();
    //core1_entry();

    // Inicia o Core 1
    multicore_launch_core1(core1_entry);

    while (1) {

    }
}

