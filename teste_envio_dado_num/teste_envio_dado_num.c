#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define I2C_SDA_PIN 0
#define I2C_SCL_PIN 1
#define PICO_I2C_ADDRESS 0x08

int main() {
    stdio_init_all();

    // Configuração do I2C
    i2c_init(I2C_PORT, 100 * 1000); // 100 kHz
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    // Configura o Pico como escravo I2C
    i2c_set_slave_mode(I2C_PORT, true, PICO_I2C_ADDRESS);

    while (1) {
        uint8_t data;
        // Lê dados do barramento I2C
        int bytes_read = i2c_read_blocking(I2C_PORT, PICO_I2C_ADDRESS, &data, 1, false);
        
        if (bytes_read == 1) {
            printf("Dado recebido: %d\n", data);
        }
        sleep_ms(10);
    }

    return 0;
}