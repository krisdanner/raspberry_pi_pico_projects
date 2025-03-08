#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define BUZZER_PIN 10  // GPIO do Buzzer A

#define NUM_NOTAS 32  // Número total de notas

// Frequências das notas (Hz)
int notas[NUM_NOTAS] = {
    262, 262, 0, 262, 0, 208, 262, 0, 330, 0, 392, 
    0, 262, 0, 208, 262, 0, 330, 0, 392, 0, 523, 
    0, 392, 0, 523, 0, 587, 0, 659, 0, 698
};

// Duração das notas (ms)
int duracoes[NUM_NOTAS] = {
    150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 300, 
    150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 300, 
    150, 150, 150, 150, 150, 150, 150, 150, 150, 300
};

// Função para tocar uma nota usando PWM
void tocar_nota(int freq, int duracao) {
    if (freq == 0) {  // Se for pausa
        sleep_ms(duracao);
        return;
    }

    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_set_wrap(slice_num, 1250000 / freq - 1);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, (1250000 / freq) / 2);
    pwm_set_enabled(slice_num, true);

    sleep_ms(duracao);  // Duração da nota

    pwm_set_enabled(slice_num, false);
    sleep_ms(50);  // Pequena pausa entre notas
}

int main() {
    stdio_init_all();
    
    // Configurar o GPIO21 para PWM
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.0);
    pwm_init(slice_num, &config, false);

    while (true) {
        for (int i = 0; i < NUM_NOTAS; i++) {
            tocar_nota(notas[i], duracoes[i]);
        }
        sleep_ms(1000);  // Pausa antes de repetir
    }

    return 0;
}


