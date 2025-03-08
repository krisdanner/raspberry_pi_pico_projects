#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define I2C_SDA_PIN 0
#define I2C_SCL_PIN 1
#define ARDUINO_I2C_ADDRESS 0x08

void setup_i2c() {
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
}

int main() {
    stdio_init_all();
    setup_i2c();

    // Configura o Pico como escravo I2C
    i2c_set_slave_mode(I2C_PORT, true, ARDUINO_I2C_ADDRESS);

    while (1) {
       uint8_t buffer[2];
        int ret = i2c_read_blocking(I2C_PORT, ARDUINO_I2C_ADDRESS, buffer, 2, false);
        if (ret == 2) {
            int16_t y_scaled = (int16_t)((buffer[0] << 8) | buffer[1]); // Reconstrução do número com sinal
            float y = y_scaled / 10000.0; // Convertendo de volta para float
            printf("y = %.4f\n", y);
        }
        sleep_ms(1);
    }
    return 0;
}
