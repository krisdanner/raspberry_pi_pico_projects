#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define I2C_SDA_PIN 0
#define I2C_SCL_PIN 1
#define ARDUINO_I2C_ADDRESS 0x08

// Parâmetros do Filtro de Kalman
float Phim[3][3] = {{0.8, 0.1, 0.005}, {0, 0.7, 0.1}, {0, 0, 0.9}};
float Gamam[3][3] = {{1, 0.05, 0.0025}, {0, 1, 0.05}, {0, 0, 1}};
float Upsilonm[3][3] = {{0.2, 0, 0}, {0, 0.2, 0}, {0, 0, 0.2}};
float Hm[3] = {1, 0.95, 0.9};
float Q[3][3] = {{10, 0, 0}, {0, 10, 0}, {0, 0, 10}};
float R = 0.1;

// Variáveis do Filtro de Kalman
float hat_x_ma[3] = {0, 0, 0}; // Estado estimado a posteriori
float P_ma[3][3] = {{100, 0, 0}, {0, 100, 0}, {0, 0, 100}}; // Covariância a posteriori

// Função para multiplicação de matrizes 3x3
void matrix_mult(float A[3][3], float B[3][3], float result[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result[i][j] = 0;
            for (int k = 0; k < 3; k++) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Função para transposição de matriz 3x3
void matrix_transpose(float A[3][3], float result[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result[j][i] = A[i][j];
        }
    }
}

// Função para calcular traço da matriz
float trace(float A[3][3]) {
    return A[0][0] + A[1][1] + A[2][2];
}

// Função para inversa de matriz 1x1 (escalar)
float inv(float scalar) {
    return 1.0 / scalar;
}

void kalman_filter(float y, float u[3]) {
    // Predição (Equação de estado)
    float hat_x_me[3] = {0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            hat_x_me[i] += Phim[i][j] * hat_x_ma[j] + Gamam[i][j] * u[j];
        }
    }

    // Predição da covariância
    float P_me[3][3] = {0};
    float temp1[3][3], temp2[3][3];
    matrix_mult(Phim, P_ma, temp1);
    matrix_mult(temp1, Phim, temp2);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            P_me[i][j] = temp2[i][j] + Upsilonm[i][j] * Q[i][j] * Upsilonm[i][j];
        }
    }

    // Cálculo do Ganho de Kalman
    float HmPme[3] = {0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            HmPme[i] += Hm[j] * P_me[j][i];
        }
    }

    float S = 0;
    for (int i = 0; i < 3; i++) {
        S += HmPme[i] * Hm[i];
    }
    S += R;

    float K[3] = {0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            K[i] += P_me[i][j] * Hm[j];
        }
        K[i] *= inv(S);
    }

    // Atualização do estado estimado
    float y_pred = 0;
    for (int i = 0; i < 3; i++) {
        y_pred += Hm[i] * hat_x_me[i];
    }
    float eta = y - y_pred;

    for (int i = 0; i < 3; i++) {
        hat_x_ma[i] = hat_x_me[i] + K[i] * eta;
    }

    // Atualização da covariância
    float KH[3][3] = {0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            KH[i][j] = K[i] * Hm[j];
        }
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            P_ma[i][j] = P_me[i][j] - KH[i][j] * P_me[i][j];
        }
    }
}

int main() {
    stdio_init_all();
    i2c_init(I2C_PORT, 2000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    // Configura o Pico como escravo I2C
        i2c_set_slave_mode(I2C_PORT, true, ARDUINO_I2C_ADDRESS);

    while (1) {
        
        uint8_t rx_data[8];
        int ret = i2c_read_blocking(I2C_PORT, ARDUINO_I2C_ADDRESS, rx_data, 8, false);

        if (ret == 8) {
            // Conversão correta dos dados recebidos
            int16_t y_raw = (int16_t)((rx_data[0] << 8) | rx_data[1]);
            int16_t u0_raw = (int16_t)((rx_data[2] << 8) | rx_data[3]);
            int16_t u1_raw = (int16_t)((rx_data[4] << 8) | rx_data[5]);
            int16_t u2_raw = (int16_t)((rx_data[6] << 8) | rx_data[7]);

            float y = y_raw / 10000.0f;
            float u[3] = {u0_raw / 10000.0f, u1_raw / 10000.0f, u2_raw / 10000.0f};

            // Executa o Filtro de Kalman
            kalman_filter(y, u);

            printf("y: %.4f | u(0): %.4f | Est: %.4f, %.4f, %.4f\n", y, u[0], hat_x_ma[0], hat_x_ma[1], hat_x_ma[2]);               
       }
        sleep_ms(1);
    }

    return 0;
}