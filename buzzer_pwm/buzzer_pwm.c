#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define BUZZER_A_PIN 21
#define BUZZER_B_PIN 10

// Notas de "Ode à Alegria" (Beethoven)
const uint16_t melody[] = {
    392, 392, 440, 392, 440, 392, 330, 349, 392, 392, 440, 392, 440, 392, 330, 349,
    392, 330, 330, 349, 392, 440, 392, 349, 392, 330, 392, 392, 440, 392, 440, 392
};

const uint16_t durations[] = {
    300, 300, 300, 300, 300, 300, 300, 300, 400, 300, 300, 300, 300, 300, 300, 300,
    400, 300, 300, 300, 400, 400, 400, 300, 300, 400, 400, 300, 300, 300, 300, 300
};

void pwm_init_buzzer(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.0f);
    pwm_init(slice_num, &config, true);
    pwm_set_gpio_level(pin, 0);
}

void play_tone(uint pin1, uint pin2, uint frequency, uint duration_ms) {
    if (frequency == 0) {
        sleep_ms(duration_ms);
        return;
    }
    uint slice_num1 = pwm_gpio_to_slice_num(pin1);
    uint slice_num2 = pwm_gpio_to_slice_num(pin2);
    uint32_t clock_freq = clock_get_hz(clk_sys);
    uint32_t top = clock_freq / (frequency * 2) - 1;
    
    pwm_set_wrap(slice_num1, top);
    pwm_set_gpio_level(pin1, top / 2);
    pwm_set_wrap(slice_num2, top);
    pwm_set_gpio_level(pin2, top / 2);
    
    sleep_ms(duration_ms);
    
    pwm_set_gpio_level(pin1, 0);
    pwm_set_gpio_level(pin2, 0);
    sleep_ms(50);
}

void play_melody(uint pin1, uint pin2) {
    size_t length = sizeof(melody) / sizeof(melody[0]);
    for (size_t i = 0; i < length; i++) {
        play_tone(pin1, pin2, melody[i], durations[i]);
    }
}

int main() {
    stdio_init_all();
    pwm_init_buzzer(BUZZER_A_PIN);
    pwm_init_buzzer(BUZZER_B_PIN);
    while (1) {
        play_melody(BUZZER_A_PIN, BUZZER_B_PIN);
        sleep_ms(1000);
    }
    return 0;
}
