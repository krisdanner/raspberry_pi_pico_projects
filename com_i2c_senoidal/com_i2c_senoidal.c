#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define I2C_SDA_PIN 0
#define I2C_SCL_PIN 1
#define ARDUINO_I2C_ADDRESS 0x08

void generate_sine_wave(float *buffer, int length, float amplitude, float frequency, float sample_rate) {
    float step = 2 * M_PI * frequency / sample_rate;
    for (int i = 0; i < length; i++) {
        buffer[i] = amplitude * sin(step * i);
    }
}

int main() {
    stdio_init_all();

    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    const int buffer_length = 100;
    float sine_wave[buffer_length];
    generate_sine_wave(sine_wave, buffer_length, 1.0, 1.0, 100.0);

    while (1) {
        for (int i = 0; i < buffer_length; i++) {
            uint8_t data = (uint8_t)((sine_wave[i] + 1.0) * 127.5); // Convertendo para 0-255
            i2c_write_blocking(I2C_PORT, ARDUINO_I2C_ADDRESS, &data, 1, false);
            sleep_ms(10); // Ajuste o tempo conforme necessário
        }
    }

    return 0;
}
