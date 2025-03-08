#include "pico/stdlib.h"  // Biblioteca padrão do Raspberry Pi Pico para entrada/saída e tempo
#include <ssd1306.h>      // Biblioteca para controlar o display OLED SSD1306
#include <stdio.h>        // Biblioteca padrão para entrada e saída de dados
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "ssd1306.h"

#define I2C_PORT i2c1
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Definição dos GPIOs
#define JOYSTICK_X_PIN 27
#define JOYSTICK_Y_PIN 26
#define JOYSTICK_SW_PIN 22
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6
#define LED_R_PIN 13
#define LED_G_PIN 11
#define LED_B_PIN 12

// Estrutura para representar a nave
typedef struct {
    int x, y;
} Ship;

Ship player = {10, 32}; // Inicializa a nave na posição x=10 e y=32 da tela
ssd1306_t oled; // Estrutura para controlar o display OLED

void setup_hardware() {
    stdio_init_all(); // Inicializa a comunicação serial
    sleep_ms(2000);

    // Configurar I2C e OLED
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    gpio_pull_up(14);
    gpio_pull_up(15);
    
    oled.external_vcc = false;
    ssd1306_init(&oled, SCREEN_WIDTH, SCREEN_HEIGHT, 0x3C, I2C_PORT);
    ssd1306_clear(&oled);
    
    // Configurar ADC para o joystick
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);
    
    // Configurar botões
    gpio_init(JOYSTICK_SW_PIN);
    gpio_set_dir(JOYSTICK_SW_PIN, GPIO_IN);
    gpio_pull_up(JOYSTICK_SW_PIN);
    
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    
    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);
    
    // Configurar LED RGB
    gpio_init(LED_R_PIN);
    gpio_init(LED_G_PIN);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);
    
    // Acender LED verde ao ligar
    gpio_put(LED_R_PIN, 0);
    gpio_put(LED_G_PIN, 0);
    gpio_put(LED_B_PIN, 0);
}

// Função para atualizar a posição da nave com base no joystick
void update_ship_position() {
    adc_select_input(1);
    int x_val = adc_read();
    adc_select_input(0);
    int y_val = adc_read();
    
    if (x_val < 1000) player.x -= 1;  // Esquerda
    if (x_val > 3000) player.x += 1;  // Direita
    if (y_val < 1000) player.y += 1;  // Cima
    if (y_val > 3000) player.y -= 1;  // Baixo
}

// Função para desenhar a nave na tela
void render_game() {
    ssd1306_clear(&oled);
    ssd1306_draw_pixel(&oled, player.x, player.y); // Desenha um pixel representando a nave
    ssd1306_show(&oled);
}

int main() {
    setup_hardware();

    while (true) {
        update_ship_position(); // Atualiza posição da nave
        render_game(); // Desenha a nave
        sleep_ms(50); // Pequeno atraso para suavizar a animação
    }
}
