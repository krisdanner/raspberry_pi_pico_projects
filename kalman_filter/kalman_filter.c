#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#define N 3
#define M 300

// Matrizes do sistema
float Phi[N][N] = {{0.8, 0.1, 0.005}, {0, 0.7, 0.1}, {0, 0, 0.9}};
float Gama[N][N] = {{1, 0.05, 0.0025}, {0, 1, 0.05}, {0, 0, 1}};
float Upsilon[N][N] = {{0.2, 0, 0}, {0, 0.2, 0}, {0, 0, 0.2}};
float H[N] = {1, 1, 1};

// Matrizes do modelo do filtro de Kalman
float Phim[N][N];
float Gamam[N][N];
float Upsilonm[N][N];
float Hm[N];

// Vetores de estado e ruído
float x[N][M+1];
float u[N][M];
float w[N][M];
float v[M];
float y[M];

// Variáveis do filtro de Kalman
float hat_x_ma[N][M];
float P_ma[N][N];
float hat_x_me[N][M];
float P_me[N][N];
float K[N];
float eta[M];
float traco[M];

// Função para inicializar matrizes com ruído
void initialize_matrices_with_noise(float q) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            Phim[i][j] = Phi[i][j] + q * ((float)rand() / RAND_MAX - 0.5);
            Gamam[i][j] = Gama[i][j] + q * ((float)rand() / RAND_MAX - 0.5);
            Upsilonm[i][j] = Upsilon[i][j] + q * ((float)rand() / RAND_MAX - 0.5);
        }
        Hm[i] = H[i] + q * ((float)rand() / RAND_MAX - 0.5);
    }
}

// Função para multiplicação de matrizes
void matrix_multiply(float A[N][N], float B[N][N], float result[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            result[i][j] = 0;
            for (int k = 0; k < N; k++) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Função para multiplicação de matriz por vetor
void matrix_vector_multiply(float A[N][N], float B[N], float result[N]) {
    for (int i = 0; i < N; i++) {
        result[i] = 0;
        for (int j = 0; j < N; j++) {
            result[i] += A[i][j] * B[j];
        }
    }
}

// Função para adição de vetores
void vector_add(float A[N], float B[N], float result[N]) {
    for (int i = 0; i < N; i++) {
        result[i] = A[i] + B[i];
    }
}

// Função para adição de matrizes
void matrix_add(float A[N][N], float B[N][N], float result[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            result[i][j] = A[i][j] + B[i][j];
        }
    }
}

// Função para cálculo do traço de uma matriz
float trace(float A[N][N]) {
    float tr = 0;
    for (int i = 0; i < N; i++) {
        tr += A[i][i];
    }
    return tr;
}

int main() {
    stdio_init_all();

    // Inicialização das variáveis
    float q = 0.1;
    float Q[N][N] = {{10, 0, 0}, {0, 10, 0}, {0, 0, 10}};
    float r = 0.1;
    float R = r;

    initialize_matrices_with_noise(q);

    // Inicialização dos vetores de estado e ruído
    for (int i = 0; i < N; i++) {
        x[i][0] = ((float)rand() / RAND_MAX - 0.5) * 0.1;
        for (int j = 0; j < M; j++) {
            u[i][j] = (float)rand() / RAND_MAX - 0.5;
            w[i][j] = (float)rand() / RAND_MAX - 0.5;
        }
    }
    for (int j = 0; j < M; j++) {
        v[j] = ((float)rand() / RAND_MAX - 0.5) * 0.5;
    }

    // Simulação da equação de estados
    for (int k = 0; k < M; k++) {
        float temp[N];
        float temp2[N];

        // Multiplicação de matriz por vetor: Phi * x[k]
        matrix_vector_multiply(Phi, x[k], temp2);

        // Multiplicação de matriz por vetor: Gama * u[k]
        matrix_vector_multiply(Gama, u[k], temp);

        // Adição de vetores: (Phi * x[k]) + (Gama * u[k])
        vector_add(temp2, temp, temp);

        // Multiplicação de matriz por vetor: Upsilon * w[k]
        matrix_vector_multiply(Upsilon, w[k], temp2);

        // Adição de vetores: (Phi * x[k] + Gama * u[k]) + (Upsilon * w[k])
        vector_add(temp, temp2, x[k+1]);
    }

    // Equação de saída
    for (int k = 0; k < M; k++) {
        y[k] = 0;
        for (int i = 0; i < N; i++) {
            y[k] += H[i] * x[i][k];
        }
        y[k] += v[k];
    }

    // Inicialização do filtro de Kalman
    for (int i = 0; i < N; i++) {
        hat_x_ma[i][0] = 0;
        for (int j = 0; j < N; j++) {
            P_ma[i][j] = (i == j) ? 100 : 0;
        }
    }

    // Filtragem de Kalman
    for (int k = 0; k < M-1; k++) {
        // Predição
        matrix_vector_multiply(Phim, hat_x_ma[k], hat_x_me[k+1]);
        matrix_multiply(Phim, P_ma, P_me);
        matrix_multiply(P_me, Phim, P_me);

        float temp[N][N];
        matrix_multiply(Upsilonm, Q, temp);
        matrix_multiply(temp, Upsilonm, temp);
        matrix_add(P_me, temp, P_me);

        // Atualização
        float S = 0;
        for (int i = 0; i < N; i++) {
            S += Hm[i] * P_me[i][i] * Hm[i];
        }
        S += R;

        for (int i = 0; i < N; i++) {
            K[i] = 0;
            for (int j = 0; j < N; j++) {
                K[i] += P_me[i][j] * Hm[j];
            }
            K[i] /= S;
        }

        eta[k+1] = y[k+1];
        for (int i = 0; i < N; i++) {
            eta[k+1] -= Hm[i] * hat_x_me[i][k+1];
        }

        for (int i = 0; i < N; i++) {
            hat_x_ma[i][k+1] = hat_x_me[i][k+1] + K[i] * eta[k+1];
        }

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                P_ma[i][j] = P_me[i][j] - K[i] * Hm[j] * P_me[i][j];
            }
        }

        traco[k] = trace(P_ma);
    }

    // Exibição dos resultados via serial
    for (int k = 0; k < M; k++) {
        printf("k = %d, x1 = %f, x2 = %f, x3 = %f, hat_x1 = %f, hat_x2 = %f, hat_x3 = %f\n", 
               k, x[0][k], x[1][k], x[2][k], hat_x_ma[0][k], hat_x_ma[1][k], hat_x_ma[2][k]);
    }

    return 0;
}