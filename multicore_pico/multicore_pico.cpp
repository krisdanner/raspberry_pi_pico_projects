#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pio.h"
#include "WS2812.hpp"

#define LED_PIN 7        // Pino de dados da matriz de LEDs WS2812B
#define LED_LENGTH 25    // Número total de LEDs

// Objeto global para controle da matriz de LEDs
WS2812 ledStrip(LED_PIN, LED_LENGTH, pio0, 0, WS2812::FORMAT_GRB);

// Função executada no Core 1
void core1_entry() {
    while (1) {
        // Pisca a segunda metade dos LEDs em azul
        ledStrip.fill(WS2812::RGB(0, 0, 255), LED_LENGTH / 2);  // Define azul na segunda metade
        ledStrip.show();
        sleep_ms(500);

        ledStrip.fill(WS2812::RGB(0, 0, 0), LED_LENGTH / 2);  // Desliga a segunda metade
        ledStrip.show();
        sleep_ms(500);
    }
}

int main() {
    stdio_init_all();

    printf("Iniciando controle de LEDs com multicore\n");

    // Inicializa o segundo núcleo (Core 1)
    multicore_launch_core1(core1_entry);

    while (1) {
        // Pisca a primeira metade dos LEDs em vermelho
        ledStrip.fill(WS2812::RGB(0, 0, 0), 0, LED_LENGTH / 2);  // Desliga a primeira metade
        ledStrip.show();
        sleep_ms(500);

        ledStrip.fill(WS2812::RGB(255, 0, 0), 0, LED_LENGTH / 2);  // Define vermelho na primeira metade
        ledStrip.show();
        sleep_ms(500);  
    }

    return 0;
}

