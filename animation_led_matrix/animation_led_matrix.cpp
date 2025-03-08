#include <stdio.h>
#include <cmath>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "WS2812.hpp"

#define LED_PIN 7
#define LED_LENGTH 25

int main()
{
    stdio_init_all();

    // 0. Initialize LED strip
    printf("0. Initialize LED strip");
    WS2812 ledStrip(
        LED_PIN,            // Define o pino GPIO onde os LEDs estão conectados
        LED_LENGTH,         // Define a quantidade de LEDs na matriz
        pio0,               // Especifica que o PIO0 será usado para o controle dos LEDs
        0,                  // Usa a máquina de estado 0 do PIO0
                            // (Cada bloco PIO tem até 4 máquinas de estado, totalizando 8 no RP2040)
                            // Consulte o Capítulo 3 em: https://datasheets.raspberrypi.org/rp2040/rp2040-datasheet.pdf
        WS2812::FORMAT_GRB  // Define o formato de cor do LED (verde, vermelho, azul - GRB)
    ); 

    // 1. Set all LEDs to red!
    printf("1. Set all LEDs to red!\n"); 
    ledStrip.fill( WS2812::RGB(255, 0, 0) ); // Define todos os LEDs como vermelho
    ledStrip.show(); // Atualiza a matriz de LEDs para refletir a mudança de cor
    sleep_ms(1000); // Aguarda 1 segundo

    // 2. Set all LEDs to green!
    printf("2. Set all LEDs to green!\n");
    ledStrip.fill( WS2812::RGB(0, 255, 0) );
    ledStrip.show();
    sleep_ms(1000);

    // 3. Set all LEDs to blue!
    printf("3. Set all LEDs to blue!\n");
    ledStrip.fill( WS2812::RGB(0, 0, 255) );
    ledStrip.show();
    sleep_ms(1000);

    // 4. Set half LEDs to red and half to blue!
    printf("4. Set half LEDs to red and half to blue!\n");
    ledStrip.fill( WS2812::RGB(255, 0, 0), 0, LED_LENGTH / 2 ); // Primeira metade em vermelho
    ledStrip.fill( WS2812::RGB(0, 0, 255), LED_LENGTH / 2 ); // Segunda metade em azul
    ledStrip.show(); // Atualiza a matriz de LEDs
    sleep_ms(1000);  // Aguarda 1 segundo
 
    // 5. Do some fancy animation
    printf("5. Do some fancy animation\n");
    while (true) {
        // Pick a random color
        uint32_t color = (uint32_t)rand();
        // Pick a random direction
        int8_t dir = (rand() & 1 ? 1 : -1);
        // Setup start and end offsets for the loop
        uint8_t start = (dir > 0 ? 0 : LED_LENGTH);
        uint8_t end = (dir > 0 ? LED_LENGTH : 0);
        for (uint8_t ledIndex = start; ledIndex != end; ledIndex += dir) {
            ledStrip.setPixelColor(ledIndex, color);
            ledStrip.show();
            sleep_ms(50);
        }
    }
    return 0;
}
