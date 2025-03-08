#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define SDA_PIN 0
#define SCL_PIN 1
#define ARDUINO_ADDR 0x08

void send_message(const char *message) {
    i2c_write_blocking(I2C_PORT, ARDUINO_ADDR, (uint8_t *)message, strlen(message), false);
}

int main() {
    stdio_init_all();

    // Inicializa o I2C
    i2c_init(I2C_PORT, 100000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    printf("Iniciando comunicação I2C com Arduino...\n");

    while (1) {
        const char *message = "Hello Arduino!";
        send_message(message);
        printf("Mensagem enviada: %s\n", message);
        sleep_ms(2000);
    }

    return 0;
}