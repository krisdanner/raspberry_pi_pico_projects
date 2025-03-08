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

// Função para calcular traço da matriz
float trace(float A[3][3]) {
    return A[0][0] + A[1][1] + A[2][2];
}

void kalman_filter(float y, float u[3]) {
    // Predição do estado
    float hat_x_me[3] = {0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            hat_x_me[i] += Phim[i][j] * hat_x_ma[j] + Gamam[i][j] * u[j];
        }
    }

    // Predição da covariância
    float P_me[3][3] = {0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            P_me[i][j] = 0;
            for (int k = 0; k < 3; k++) {
                P_me[i][j] += Phim[i][k] * P_ma[k][j];
            }
            P_me[i][j] += Upsilonm[i][j] * Q[i][j] * Upsilonm[i][j];
        }
    }

    // Cálculo do Ganho de Kalman
    float S = R;
    float K[3] = {0};
    for (int i = 0; i < 3; i++) {
        float sum = 0;
        for (int j = 0; j < 3; j++) {
            sum += Hm[j] * P_me[j][i];
        }
        S += sum * Hm[i];
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            K[i] += P_me[i][j] * Hm[j];
        }
        K[i] /= S;
    }

    // Atualização do estado estimado
    float y_pred = 0;
    for (int i = 0; i < 3; i++) y_pred += Hm[i] * hat_x_me[i];
    float eta = y - y_pred;
    for (int i = 0; i < 3; i++) hat_x_ma[i] = hat_x_me[i] + K[i] * eta;

    // Atualização da covariância
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            P_ma[i][j] = P_me[i][j] - K[i] * Hm[j] * P_me[i][j];
        }
    }
}

int main() {
    stdio_init_all();
    i2c_init(I2C_PORT, 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    i2c_set_slave_mode(I2C_PORT, true, ARDUINO_I2C_ADDRESS);

    while (1) {
        uint8_t rx_data[8];
        if (i2c_read_blocking(I2C_PORT, ARDUINO_I2C_ADDRESS, rx_data, 8, false) == 8) {
            float y = ((int16_t)(rx_data[0] << 8 | rx_data[1])) / 100.0;
            float u[3] = {
                ((int16_t)(rx_data[2] << 8 | rx_data[3])) / 100.0,
                ((int16_t)(rx_data[4] << 8 | rx_data[5])) / 100.0,
                ((int16_t)(rx_data[6] << 8 | rx_data[7])) / 100.0};

            kalman_filter(y, u);
            int16_t tx_data[4] = {
                (int16_t)(hat_x_ma[0] * 100),
                (int16_t)(hat_x_ma[1] * 100),
                (int16_t)(hat_x_ma[2] * 100),
                (int16_t)(trace(P_ma) * 100)};

            uint8_t tx_bytes[8];
            for (int i = 0; i < 4; i++) {
                tx_bytes[2 * i] = (tx_data[i] >> 8) & 0xFF;
                tx_bytes[2 * i + 1] = tx_data[i] & 0xFF;
            }
            //i2c_write_blocking(I2C_PORT, ARDUINO_I2C_ADDRESS, tx_bytes, 8, false);
            printf("y: %.4f | u(0): %.4f | x1: %.4f | x2: %.4f | x3: %.4f | Trace(P_ma): %.4f\n",
                   y, u[0], hat_x_ma[0], hat_x_ma[1], hat_x_ma[2], trace(P_ma));
        }
        sleep_ms(1);
    }
    return 0;
}

