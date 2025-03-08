// Display OLED 128x64
// Christian Danner Ramos de Carvalho
// 27/02/2025

#include "pico/stdlib.h"  // Biblioteca padrão do Raspberry Pi Pico para entrada/saída e tempo
#include <ssd1306.h>      // Biblioteca para controlar o display OLED SSD1306
#include <stdio.h>        // Biblioteca padrão para entrada e saída de dados

#define DELAY 500 // Tempo de espera entre atualizações, em milissegundos

/***
 * Desenha a palavra "Testando..." na tela OLED e uma linha horizontal abaixo dela.
 */
void drawTest(ssd1306_t *pOled) {
    ssd1306_draw_string(pOled, 0, 1, 2, "Teste..."); // Escreve "Testando..." na posição (0,1) com tamanho 2
    ssd1306_draw_line(pOled, 0, 20, 163, 20); // Desenha uma linha horizontal de (0,20) até (163,20)
}

/***
 * Desenha "Testando..." seguido pelos nomes "Christian" e "Danner" na tela OLED.
 */
void drawBlink(ssd1306_t *pOled) {
    drawTest(pOled);  // Chama a função drawTest() para desenhar "Testando..."
    ssd1306_draw_string(pOled, 2, 25, 2, "Alinne &"); // Escreve "Christian" na posição (2,25) com tamanho 2
    ssd1306_draw_string(pOled, 2, 45, 2, "Arthur");   // Escreve "Danner" na posição (2,45) com tamanho 2
}

int main() {
    stdio_init_all(); // Inicializa a entrada/saída padrão
    sleep_ms(2000); // Aguarda 2 segundos para inicializar tudo corretamente
    puts("GO"); // Imprime "GO" no console

    // Configuração do barramento I2C para comunicação com o display OLED
    i2c_init(i2c1, 400000); // Inicializa o barramento I2C1 com frequência de 400 kHz
    gpio_pull_up(14); // Habilita resistor de pull-up no pino 14 (SDA)
    gpio_pull_up(15); // Habilita resistor de pull-up no pino 15 (SCL)
    gpio_set_function(14, GPIO_FUNC_I2C); // Define o pino 14 como função I2C (SDA)
    gpio_set_function(15, GPIO_FUNC_I2C); // Define o pino 15 como função I2C (SCL)

    ssd1306_t oled; // Declara a estrutura para armazenar os dados do display OLED
    oled.external_vcc = 0;  // Define o uso de alimentação interna do display (VCC gerado internamente)

    // Inicializa o display OLED com resolução de 128x64, endereço I2C 0x3C e usando o barramento i2c1
    int res = ssd1306_init(&oled, 128, 64, 0x3C, i2c1);

    // Se a inicialização foi bem-sucedida, escreve "Testando..." no display
    if (res) {
        ssd1306_clear(&oled); // Limpa a tela OLED
        drawTest(&oled); // Desenha "Testando..."
        ssd1306_show(&oled); // Atualiza a tela para exibir os desenhos
    } else {
        puts("Oled Init failed"); // Se houver falha, imprime mensagem de erro
    }

    const uint LED_VERDE = 11; // Define o pino 11 como saída para o LED verde do RGB
    gpio_init(LED_VERDE); // Inicializa o pino 11
    gpio_set_dir(LED_VERDE, GPIO_OUT); // Configura o pino como saída

    const uint LED_AZUL = 12; // Define o pino 11 como saída para o LED azul do RGB
    gpio_init(LED_AZUL); // Inicializa o pino 11
    gpio_set_dir(LED_AZUL, GPIO_OUT); // Configura o pino como saída

    const uint LED_VERMELHO = 13; // Define o pino 11 como saída para o LED verde do RGB
    gpio_init(LED_VERMELHO); // Inicializa o pino 11
    gpio_set_dir(LED_VERMELHO, GPIO_OUT); // Configura o pino como saída

    // Loop infinito alternando entre os dois desenhos e piscando o LED
    while (1) {
        gpio_put(LED_VERDE, 1); // Liga o LED (nível alto)
        ssd1306_clear(&oled); // Limpa a tela OLED
        drawBlink(&oled); // Desenha "Testando..." + "Christian" + "Danner"
        ssd1306_show(&oled); // Atualiza a tela
        sleep_ms(DELAY); // Aguarda o tempo definido em DELAY

        gpio_put(LED_VERDE, 0); // Desliga o LED (nível baixo)
        ssd1306_clear(&oled); // Limpa a tela OLED
        drawTest(&oled); // Desenha apenas "Testando..."
        ssd1306_show(&oled); // Atualiza a tela
        sleep_ms(DELAY); // Aguarda novamente o tempo definido em DELAY

        gpio_put(LED_AZUL, 1); // Liga o LED (nível alto)
        ssd1306_clear(&oled); // Limpa a tela OLED
        drawBlink(&oled); // Desenha "Testando..." + "Christian" + "Danner"
        ssd1306_show(&oled); // Atualiza a tela
        sleep_ms(DELAY); // Aguarda o tempo definido em DELAY

        gpio_put(LED_AZUL, 0); // Desliga o LED (nível baixo)
        ssd1306_clear(&oled); // Limpa a tela OLED
        drawTest(&oled); // Desenha apenas "Testando..."
        ssd1306_show(&oled); // Atualiza a tela
        sleep_ms(DELAY); // Aguarda novamente o tempo definido em DELAY

        gpio_put(LED_VERMELHO, 1); // Liga o LED (nível alto)
        ssd1306_clear(&oled); // Limpa a tela OLED
        drawBlink(&oled); // Desenha "Testando..." + "Christian" + "Danner"
        ssd1306_show(&oled); // Atualiza a tela
        sleep_ms(DELAY); // Aguarda o tempo definido em DELAY

        gpio_put(LED_VERMELHO, 0); // Desliga o LED (nível baixo)
        ssd1306_clear(&oled); // Limpa a tela OLED
        drawTest(&oled); // Desenha apenas "Testando..."
        ssd1306_show(&oled); // Atualiza a tela
        sleep_ms(DELAY); // Aguarda novamente o tempo definido em DELAY
    }

    return 0; // Retorno padrão da função main (nunca será alcançado neste código)
}


