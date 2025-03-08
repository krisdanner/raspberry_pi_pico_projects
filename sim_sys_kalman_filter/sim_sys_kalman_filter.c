#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Definições do I2C
#define I2C_PORT i2c0
#define I2C_SDA_PIN 0
#define I2C_SCL_PIN 1
#define ARDUINO_ADDR 0x08 // Endereço I2C do Arduino

// Parâmetros do sistema
const float dt = 0.1;
const float A[2][2] = {{1, dt}, {0, 1}};
const float B[2] = {dt * dt / 2, dt};
const float u = 0.1;
const float Q[2][2] = {{0.01 * 0.01, 0}, {0, 0.02 * 0.02}}; // Matriz de covariância do sistema
const float R = 25; // Variância do ruído do sensor (sigma_v^2)
const float C[2] = {1, 0}; // Matriz de observação

// Variáveis do Filtro de Kalman
float xK[2] = {30, 0}; // Estado inicial estimado
float P[2][2] = {{Q[0][0], 0}, {0, Q[1][1]}}; // Covariância inicial

// Função para multiplicação de matrizes 2x2
void mat_mult(const float a[2][2], const float b[2][2], float result[2][2]) {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            result[i][j] = 0;
            for (int k = 0; k < 2; k++) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

// Função para multiplicação de matriz por vetor
void mat_vec_mult(const float a[2][2], const float b[2], float result[2]) {
    for (int i = 0; i < 2; i++) {
        result[i] = 0;
        for (int j = 0; j < 2; j++) {
            result[i] += a[i][j] * b[j];
        }
    }
}

// Função para transposição de matriz
void mat_transpose(const float a[2][2], float result[2][2]) {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            result[i][j] = a[j][i];
        }
    }
}

// Função do Filtro de Kalman
float kalman_filter(float y) {
    // Predição do estado
    float xK_pred[2];
    mat_vec_mult(A, xK, xK_pred);
    xK_pred[0] += B[0] * u;
    xK_pred[1] += B[1] * u;

    // Predição da covariância
    float P_pred[2][2];
    mat_mult(A, P, P_pred);
    mat_transpose(A, P_pred);
    mat_mult(P_pred, A, P_pred);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            P_pred[i][j] += Q[i][j];
        }
    }

    // Cálculo do ganho de Kalman
    float K[2];
    float denom = C[0] * P_pred[0][0] * C[0] + R;
    K[0] = P_pred[0][0] * C[0] / denom;
    K[1] = P_pred[1][0] * C[0] / denom;

    // Atualização do estado estimado
    xK[0] = xK_pred[0] + K[0] * (y - C[0] * xK_pred[0]);
    xK[1] = xK_pred[1] + K[1] * (y - C[0] * xK_pred[0]);

    // Atualização da covariância
    float I[2][2] = {{1, 0}, {0, 1}};
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            P[i][j] = (I[i][j] - K[i] * C[j]) * P_pred[i][j];
        }
    }

    return xK[0]; // Retorna o estado estimado x(1)
}

// Função principal
int main() {
    stdio_init_all(); // Inicializa a comunicação serial

    // Configuração do I2C
    i2c_init(I2C_PORT, 100 * 1000); // 100 kHz
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    while (1) {
        // Recebe a leitura do sensor do Arduino via I2C
        uint8_t data[4];
        i2c_read_blocking(I2C_PORT, ARDUINO_ADDR, data, 4, false);
        float y = *((float*)data); // Converte os bytes para float

        // Recebe o estado real do sistema do Arduino via I2C
        i2c_read_blocking(I2C_PORT, ARDUINO_ADDR, data, 4, false);
        float x_real = *((float*)data); // Converte os bytes para float

        // Executa o Filtro de Kalman
        float z = kalman_filter(y);

        // Imprime os valores no monitor serial
        printf("Sensor com ruído (y): %.2f, Estado real (x_real): %.2f, Estado estimado (z): %.2f\n", y, x_real, z);

        // Envia o estado estimado de volta para o Arduino via I2C
        i2c_write_blocking(I2C_PORT, ARDUINO_ADDR, (uint8_t*)&z, sizeof(z), false);

        // Aguarda o próximo ciclo
        sleep_ms((int)(dt * 1000));
    }

    return 0;
}