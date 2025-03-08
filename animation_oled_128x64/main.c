#include "pico/stdlib.h"
#include <ssd1306.h>
#include <stdio.h>
#include "bellboy.h"

void drawBellboy(ssd1306_t *pOled, int xOffset) {
    ssd1306_clear(pOled);

    for (int y = 0; y < 64; y++) {
        for (int x = 0; x < 64; x++) {
            if ((x + xOffset) >= 0 && (x + xOffset) < 128) {
                // Byte Index
                int b = y * 8 + x / 8;
                // Mask
                int mask = 1 << (x % 8);
                if ((BELLBOY[b] & mask) > 0) {
                    ssd1306_draw_pixel(pOled, x + xOffset, y);
                }
            }
        }
    }

    ssd1306_show(pOled);
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    puts("GO");

    i2c_init(i2c1, 400000);
    gpio_pull_up(14);
    gpio_pull_up(15);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);

    ssd1306_t oled;
    oled.external_vcc = 0;  // false -> 0 em C

    int res = ssd1306_init(&oled, 128, 64, 0x3C, i2c1);

    if (res) {
        while (1) {
            for (int x = -64; x < 128; x++) {
                drawBellboy(&oled, x);
            }
        }
    } else {
        puts("Oled Init failed");
    }

    return 0;
}

