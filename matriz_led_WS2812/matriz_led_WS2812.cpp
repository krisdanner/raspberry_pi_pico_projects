#include <stdio.h>
#include <cmath>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "WS2812.hpp"

#define LED_PIN 7
#define LED_LENGTH 25

int main() {
    stdio_init_all();

    WS2812 ledStrip(
        LED_PIN,       // Pino de controle
        LED_LENGTH,    // Número de LEDs
        pio0,          // Usando PIO 0
        0,             // Máquina de estado 0
        WS2812::FORMAT_GRB
    );

    while (1) {
        ledStrip.fill(WS2812::RGB(255, 0, 0)); // Vermelho
        ledStrip.show();
        sleep_ms(1000);

        ledStrip.fill(WS2812::RGB(0, 255, 0)); // Verde
        ledStrip.show();
        sleep_ms(1000);

        ledStrip.fill(WS2812::RGB(0, 0, 255)); // Azul
        ledStrip.show();
        sleep_ms(1000);
    }

    return 0;
}


