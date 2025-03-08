#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

#define LED_PIN 12         // Pino do LED
#define BUTTON1_PIN 5      // Pino do botão 1
#define BUTTON2_PIN 6      // Pino do botão 2
#define WIFI_SSID "Redmi Note 13 Pro 5G"  // Substitua pelo nome da sua rede Wi-Fi
#define WIFI_PASS "981793734" // Substitua pela senha da sua rede Wi-Fi

// Estado dos botões
bool button1_state = false;
bool button2_state = false;

// Buffer para resposta HTTP
char http_response[1024];

// Função para criar a resposta da página HTML com AJAX
void create_http_response() {
    snprintf(http_response, sizeof(http_response),
             "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n"
             "<!DOCTYPE html>"
             "<html>"
             "<head>"
             "  <meta charset=\"UTF-8\">"
             "  <title>Controle do LED e Botões</title>"
             "  <script>"
             "    function updateButtonStates() {"
             "      fetch('/status')"
             "        .then(response => response.json())"
             "        .then(data => {"
             "          document.getElementById('button1').innerText = 'Botão 1: ' + data.button1;"
             "          document.getElementById('button2').innerText = 'Botão 2: ' + data.button2;"
             "        });"
             "    }"
             "    setInterval(updateButtonStates, 1000);"
             "  </script>"
             "</head>"
             "<body onload=\"updateButtonStates()\">"
             "  <h1>Controle do LED e Botões</h1>"
             "  <p><a href='/led/on'>Ligar LED</a></p>"
             "  <p><a href='/led/off'>Desligar LED</a></p>"
             "  <h2>Estado dos Botões:</h2>"
             "  <p id='button1'>Carregando...</p>"
             "  <p id='button2'>Carregando...</p>"
             "</body>"
             "</html>\r\n");
}

// Função para criar resposta JSON com estados dos botões
void create_json_response() {
    snprintf(http_response, sizeof(http_response),
             "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n"
             "{\"button1\": \"%s\", \"button2\": \"%s\"}",
             button1_state ? "Pressionado" : "Solto",
             button2_state ? "Pressionado" : "Solto");
}

// Função de callback para processar requisições HTTP
static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    char *request = (char *)p->payload;

    if (strstr(request, "GET /led/on")) {
        gpio_put(LED_PIN, 1);
    } else if (strstr(request, "GET /led/off")) {
        gpio_put(LED_PIN, 0);
    } else if (strstr(request, "GET /status")) {
        create_json_response();
    } else {
        create_http_response();
    }

    tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);
    pbuf_free(p);
    return ERR_OK;
}

// Callback de conexão
static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, http_callback);
    return ERR_OK;
}

// Função para iniciar o servidor HTTP
static void start_http_server(void) {
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        printf("Erro ao criar PCB\n");
        return;
    }

    if (tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK) {
        printf("Erro ao ligar o servidor na porta 80\n");
        return;
    }

    pcb = tcp_listen(pcb);
    tcp_accept(pcb, connection_callback);
    printf("Servidor HTTP rodando na porta 80...\n");
}

// Função para monitorar os botões
void monitor_buttons() {
    static bool button1_last_state = false;
    static bool button2_last_state = false;

    button1_state = !gpio_get(BUTTON1_PIN);
    button2_state = !gpio_get(BUTTON2_PIN);

    if (button1_state != button1_last_state) {
        button1_last_state = button1_state;
        printf("Botão 1: %s\n", button1_state ? "Pressionado" : "Solto");
    }

    if (button2_state != button2_last_state) {
        button2_last_state = button2_state;
        printf("Botão 2: %s\n", button2_state ? "Pressionado" : "Solto");
    }
}

int main() {
    stdio_init_all();
    sleep_ms(10000);
    printf("Iniciando servidor HTTP\n");

    if (cyw43_arch_init()) {
        printf("Erro ao inicializar o Wi-Fi\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("Falha ao conectar ao Wi-Fi\n");
        return 1;
    } else {
        printf("Connected.\n");
        // Read the ip address in a human readable way
        uint8_t *ip_address = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
        printf("Endereço IP %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }

    printf("Wi-Fi conectado!\n");

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(BUTTON1_PIN);
    gpio_set_dir(BUTTON1_PIN, GPIO_IN);
    gpio_pull_up(BUTTON1_PIN);

    gpio_init(BUTTON2_PIN);
    gpio_set_dir(BUTTON2_PIN, GPIO_IN);
    gpio_pull_up(BUTTON2_PIN);

    printf("Botões configurados nos pinos %d e %d\n", BUTTON1_PIN, BUTTON2_PIN);

    start_http_server();

    while (true) {
        cyw43_arch_poll();
        monitor_buttons();
        sleep_ms(100);
    }

    cyw43_arch_deinit();
    return 0;
}
