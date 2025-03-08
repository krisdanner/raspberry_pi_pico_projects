#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

#define RED_PIN 13         // Pino do LED Vermelho
#define GREEN_PIN 11       // Pino do LED Verde
#define BLUE_PIN 12        // Pino do LED Azul
#define WIFI_SSID "DANNER"  // Substitua pelo nome da sua rede Wi-Fi
#define WIFI_PASS "41265955" // Substitua pela senha da sua rede Wi-Fi

// Buffer para respostas HTTP
#define HTTP_RESPONSE "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" \
                      "<!DOCTYPE html><html><head>" \
                      "<style>" \
                      "body { font-size: 24px; text-align: center; }" \
                      "button { font-size: 24px; padding: 15px; margin: 10px; border: none; cursor: pointer; }" \
                      ".btn-off { background-color: gray; color: white; }" \
                      ".btn-red { background-color: red; color: white; }" \
                      ".btn-green { background-color: green; color: white; }" \
                      ".btn-blue { background-color: blue; color: white; }" \
                      "</style></head><body>" \
                      "<h1>Controle do LED RGB</h1>" \
                      "<p><a href='/led/off'><button class='btn-off'>Desligar LED</button></a></p>" \
                      "<p><a href='/led/red'><button class='btn-red'>Vermelho</button></a></p>" \
                      "<p><a href='/led/green'><button class='btn-green'>Verde</button></a></p>" \
                      "<p><a href='/led/blue'><button class='btn-blue'>Azul</button></a></p>" \
                      "<footer><p>Autor: Christian Danner Ramos de Carvalho</p>" \
                      "<p>Embarcatech - IFMA</p>" \
                      "<p>Data: 02/03/2025</p></footer>" \
                      "</body></html>\r\n"



// Função de callback para processar requisições HTTP
static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        // Cliente fechou a conexão
        tcp_close(tpcb);
        return ERR_OK;
    }

    // Processa a requisição HTTP
    char *request = (char *)p->payload;

    if (strstr(request, "GET /led/off")) {
        gpio_put(RED_PIN, 0);   // Desliga a cor vermelha
        gpio_put(GREEN_PIN, 0); // Desliga a cor verde
        gpio_put(BLUE_PIN, 0);  // Desliga a cor azul
    } else if (strstr(request, "GET /led/red")) {
        gpio_put(RED_PIN, 1);   // Liga a cor vermelha
        gpio_put(GREEN_PIN, 0); // Desliga a cor verde
        gpio_put(BLUE_PIN, 0);  // Desliga a cor azul
    } else if (strstr(request, "GET /led/green")) {
        gpio_put(RED_PIN, 0);   // Desliga a cor vermelha
        gpio_put(GREEN_PIN, 1); // Liga a cor verde
        gpio_put(BLUE_PIN, 0);  // Desliga a cor azul
    } else if (strstr(request, "GET /led/blue")) {
        gpio_put(RED_PIN, 0);   // Desliga a cor vermelha
        gpio_put(GREEN_PIN, 0); // Desliga a cor verde
        gpio_put(BLUE_PIN, 1);  // Liga a cor azul
    }

    // Envia a resposta HTTP
    tcp_write(tpcb, HTTP_RESPONSE, strlen(HTTP_RESPONSE), TCP_WRITE_FLAG_COPY);

    // Libera o buffer recebido
    pbuf_free(p);

    return ERR_OK;
}

// Callback de conexão: associa o http_callback à conexão
static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, http_callback);  // Associa o callback HTTP
    return ERR_OK;
}

// Função de setup do servidor TCP
static void start_http_server(void) {
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        printf("Erro ao criar PCB\n");
        return;
    }

    // Liga o servidor na porta 80
    if (tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK) {
        printf("Erro ao ligar o servidor na porta 80\n");
        return;
    }

    pcb = tcp_listen(pcb);  // Coloca o PCB em modo de escuta
    tcp_accept(pcb, connection_callback);  // Associa o callback de conexão

    printf("Servidor HTTP rodando na porta 80...\n");
}

int main() {
    stdio_init_all();  // Inicializa a saída padrão
    sleep_ms(10000);
    printf("Iniciando servidor HTTP\n");

    // Inicializa o Wi-Fi
    if (cyw43_arch_init()) {
        printf("Erro ao inicializar o Wi-Fi\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("Falha ao conectar ao Wi-Fi\n");
        return 1;
    }else {
        printf("Connected.\n");
        // Read the ip address in a human readable way
        uint8_t *ip_address = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
        printf("Endereço IP %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }

    printf("Wi-Fi conectado!\n");
    printf("Para ligar ou desligar o LED acesse o Endereço IP\n");

    // Configura os pinos dos LEDs como saída
    gpio_init(RED_PIN);
    gpio_set_dir(RED_PIN, GPIO_OUT);
    gpio_init(GREEN_PIN);
    gpio_set_dir(GREEN_PIN, GPIO_OUT);
    gpio_init(BLUE_PIN);
    gpio_set_dir(BLUE_PIN, GPIO_OUT);

    // Inicia o servidor HTTP
    start_http_server();
    
    // Loop principal
    while (true) {
        cyw43_arch_poll();  // Necessário para manter o Wi-Fi ativo
        sleep_ms(100);
    }

    cyw43_arch_deinit();  // Desliga o Wi-Fi (não será chamado, pois o loop é infinito)
    return 0;
}