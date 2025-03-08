#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define SDA_PIN 0
#define SCL_PIN 1
#define ARDUINO_ADDR 0x08
#define BUFFER_SIZE 32  // Tamanho máximo da mensagem recebida

void send_message(const char *message) {
    i2c_write_blocking(I2C_PORT, ARDUINO_ADDR, (uint8_t *)message, strlen(message), false);
}

void receive_message(char *buffer, size_t length) {
    memset(buffer, 0, length); // Limpa o buffer antes de usar
    int bytes_received = i2c_read_blocking(I2C_PORT, ARDUINO_ADDR, (uint8_t *)buffer, length - 1, false);
    
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0'; // Garante que a string termine corretamente
    } else {
        strcpy(buffer, "Erro na leitura!");
    }
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

    char received_msg[BUFFER_SIZE];

    while (1) {
        // Envia mensagem para o Arduino
        const char *message = "Hello Arduino!";
        send_message(message);
        printf("Mensagem enviada: %s\n", message);
        
        sleep_ms(1000); // Espera um pouco antes de tentar ler

        // Tenta receber mensagem do Arduino
        receive_message(received_msg, BUFFER_SIZE - 1);
        printf("Mensagem recebida: %s\n", received_msg);

        sleep_ms(2000);
    }
    return 0;
}
