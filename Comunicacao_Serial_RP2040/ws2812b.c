#include <stdio.h>
#include "pico/stdlib.h"

#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/i2c.h"

#include "pico/bootrom.h"

#include "features/PIO.c"
#include "features/desenho.c"
#include "features/ssd1306.h"


// Definição do número de LEDs e pino.
// Matriz
#define LED_COUNT 25
#define LED_PIN 7 

// I2C
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
ssd1306_t ssd;
bool cor = true;


// Led RGB
const uint LED_G = 11;
const uint LED_B = 12;
const uint LED_R = 13;

#define DEBOUNCE_TIME 200000 // 200ms em us de debounce
uint32_t last_time = 0;      // captura o tempo do ultimo acionamento do botão para o debounce


// Variaveis
volatile bool led_B_ativado = 0; // Usado para saber qual led esta ligado
volatile bool led_G_ativado = 0; // Usado para saber qual led esta ligado
volatile bool BUTTON_A_pressionado = false;
volatile bool BUTTON_B_pressionado = false;

// Botoes
#define BUTTON_A 5
#define BUTTON_B 6
#define BTN_STICK 22


// Funcoes
void callback_botao(uint gpio, uint32_t events);
void resposta_btns(int led);
void escreve_caractere(char c);

int main(){
    // Inicializa entradas e saídas.
    stdio_init_all();

    printf("Funciona 0\n");
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA);                                        // Pull up the data line
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SCL);                                        // Pull up the clock line
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    
    ssd1306_config(&ssd);                                         // Configura o display
    
    ssd1306_send_data(&ssd);         

    // Limpa o display. O display inicia com todos os pixels LIGADOS.
    ssd1306_fill(&ssd, true);
    ssd1306_send_data(&ssd);
    
    gpio_init(LED_B);
    gpio_set_dir(LED_B, GPIO_OUT);

    gpio_init(LED_G);
    gpio_set_dir(LED_G, GPIO_OUT);

    gpio_init(LED_R);
    gpio_set_dir(LED_R, GPIO_OUT);

    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);

    gpio_init(BTN_STICK);
    gpio_set_dir(BTN_STICK, GPIO_IN);
    gpio_pull_up(BTN_STICK);


    // Inicializa matriz de LEDs NeoPixel.
    npInit(LED_PIN);
    //função para iqr
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_RISE, true, &callback_botao);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_RISE, true, &callback_botao); 
    gpio_set_irq_enabled_with_callback(BTN_STICK, GPIO_IRQ_EDGE_FALL, true, &callback_botao); //Bootloader
    printf("Funciona 1\n");
    while (true){
            char c;
            printf("Funciona 2\n");
            if (scanf("%c", &c) == 1){ // Lê caractere da entrada padrão
                printf("Funciona 3\n");
                printf("Caractere recebido: %c\n", c);
                escreve_caractere(c);
                switch (c){
                    case '0':
                        desenhaMatriz(zero, 1, 0.8);
                        break;
                    case '1':
                        desenhaMatriz(um, 1, 0.8);
                        break;
                    case '2':
                        desenhaMatriz(dois, 1, 0.8);
                        break;
                    case '3':
                        desenhaMatriz(tres, 1, 0.8);
                        break;
                    case '4':
                        desenhaMatriz(quatro, 1, 0.8);
                        break;
                    case '5':
                        desenhaMatriz(cinco, 1, 0.8);
                        break;
                    case '6':
                        desenhaMatriz(seis, 1, 0.8);
                        break;
                    case '7':
                        desenhaMatriz(seis, 1, 0.8);
                        break;
                    case '8':
                        desenhaMatriz(oito, 1, 0.8);
                        break;
                    case '9':
                        desenhaMatriz(nove, 1, 0.8);
                        break;
                    default:
                        desenhaMatriz(desliga, 0, 0.8);
                }
            }
        sleep_ms(100); // Pequeno delay para estabilidade

    } // Fim While True
} // Fim main



//função para indicar que botão está sendo apertado
void callback_botao(uint gpio, uint32_t events){
    uint32_t current_time = to_us_since_boot(get_absolute_time()); // captura o momento do acionamento e converte para microsegundos
    if(current_time - last_time > DEBOUNCE_TIME){
        if (gpio == BUTTON_A) {
            if (led_G_ativado){ // Se o verde estiver ativo
                gpio_put(LED_G, 0);
                led_G_ativado = 0;
            }
            else{ // Se o verde estiver desativado
                gpio_put(LED_G, 1);
                led_G_ativado = 1;
            }
            resposta_btns(LED_G); // Escreve no ssd1306
        }
        else if (gpio == BUTTON_B) { 
            if (led_B_ativado){ // Se o Azul estiver ativo
                gpio_put(LED_B, 0);
                led_B_ativado = 0;
                // LED ACESO ( LIGAR SSD1306 )
            }
            else{ // Se o Azul estiver desativado
                gpio_put(LED_B, 1);
                led_B_ativado = 1;
            }
            resposta_btns(LED_B); // Escreve no ssd1306
        } // acionamento botao
        else if(gpio == BTN_STICK){
            reset_usb_boot(0, 0); //func para entrar no modo bootsel 
        }
        last_time = current_time; // Atualiza o tempo para o debounce
    } // debounce
}

void escreve_caractere(char c){ // atualiza o display com o caractere inserido pelo usuario
    ssd1306_fill(&ssd, cor);
    ssd1306_rect(&ssd, 3, 3, 122, 58, !cor, cor);
    ssd1306_draw_string(&ssd, "CARACTERE", 28, 10);
    ssd1306_draw_string(&ssd, "         ", 20, 30);
    ssd1306_draw_string(&ssd, &c, 63, 30);
    ssd1306_draw_string(&ssd, "PRESSIONADO", 20, 48);
    ssd1306_send_data(&ssd);
}

void resposta_btns(int led){ //trata o acionamento dos botoes, verificando estado e cor dos leds
    if(led == LED_G){
        if (led_G_ativado){
            ssd1306_fill(&ssd, cor);
            ssd1306_rect(&ssd, 3, 3, 122, 58, !cor, cor);
            ssd1306_draw_string(&ssd, "BOTAO A", 35, 10);
            ssd1306_draw_string(&ssd, "PRESSIONADO", 20, 30);
            ssd1306_draw_string(&ssd, "LED VERDE ON", 16, 48);
            ssd1306_send_data(&ssd);
            printf("Led VERDE ligado\n");
        }
        else{
            ssd1306_fill(&ssd, cor);
            ssd1306_rect(&ssd, 3, 3, 122, 58, !cor, cor);
            ssd1306_draw_string(&ssd, "BOTAO A", 35, 10);
            ssd1306_draw_string(&ssd, "PRESSIONADO", 20, 30);
            ssd1306_draw_string(&ssd, "LED VERDE OFF", 16, 48);
            ssd1306_send_data(&ssd);
            printf("Led VERDE desligado\n");
        }
    }
    else if(led == LED_B){
        if (led_B_ativado){
            ssd1306_fill(&ssd, cor);
            ssd1306_rect(&ssd, 3, 3, 122, 58, !cor, cor);
            ssd1306_draw_string(&ssd, "BOTAO B", 35, 10);
            ssd1306_draw_string(&ssd, "PRESSIONADO", 20, 30);
            ssd1306_draw_string(&ssd, "LED AZUL ON", 16, 48);
            ssd1306_send_data(&ssd);
            printf("Led AZUL ligado\n");
        }
        else{
            ssd1306_fill(&ssd, cor);
            ssd1306_rect(&ssd, 3, 3, 122, 58, !cor, cor);
            ssd1306_draw_string(&ssd, "BOTAO B", 35, 10);
            ssd1306_draw_string(&ssd, "PRESSIONADO", 20, 30);
            ssd1306_draw_string(&ssd, "LED AZUL OFF", 16, 48);
            ssd1306_send_data(&ssd);
            printf("Led AZUL desligado\n");
        }
    }
}