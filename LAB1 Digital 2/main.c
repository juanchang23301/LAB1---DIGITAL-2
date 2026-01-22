/************************************************************************
 Contador automático 5 a 0 con pushbutton + TIMER
 Juego de dos jugadores con inicio al llegar a 0
 ATmega328P / Arduino Nano
************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>


// Display
#define DISP_A PD6
#define DISP_B PD7
#define DISP_C PB0
#define DISP_D PB1
#define DISP_E PB2
#define DISP_F PB3
#define DISP_G PB4

// Botones
#define BTN     PD4   // Inicia contador del display
#define BTN_P1  PD3   // Jugador 1
#define BTN_P2  PD5   // Jugador 2

// LEDs Jugador 1 (PC5–PC2)
#define LED_P1_1 PC5
#define LED_P1_2 PC4
#define LED_P1_3 PC3
#define LED_P1_4 PC2

// LEDs Jugador 2 (PC1, PC0, PB5, PD2)
#define LED_P2_1 PC1
#define LED_P2_2 PC0
#define LED_P2_3 PB5
#define LED_P2_4 PD2

//Inicializar variables de contador
volatile uint8_t contador     = 5;
volatile uint8_t contando     = 0;

volatile uint8_t contador_P1  = 0;
volatile uint8_t contador_P2  = 0;

volatile uint8_t ganador = 0;          
volatile uint8_t iniciar_juego = 0;

// Tabla de segmentos
const uint8_t numeros[6][7] = {
    {1,1,1,1,1,1,0}, // 0
    {0,1,1,0,0,0,0}, // 1
    {1,1,0,1,1,0,1}, // 2
    {1,1,1,1,0,0,1}, // 3
    {0,1,1,0,0,1,1}, // 4
    {1,0,1,1,0,1,1}  // 5
};

//Inicializar pines del diplay
void setup_pins(void) {
    DDRD |= (1 << DISP_A) | (1 << DISP_B);
    DDRB |= (1 << DISP_C) | (1 << DISP_D) |
            (1 << DISP_E) | (1 << DISP_F) |
            (1 << DISP_G);

    DDRD &= ~((1 << BTN) | (1 << BTN_P1) | (1 << BTN_P2));
    PORTD |=  (1 << BTN) | (1 << BTN_P1) | (1 << BTN_P2);

    DDRC |= (1 << LED_P1_1) | (1 << LED_P1_2) |
            (1 << LED_P1_3) | (1 << LED_P1_4) |
            (1 << LED_P2_1) | (1 << LED_P2_2);

    DDRB |= (1 << LED_P2_3);
    DDRD |= (1 << LED_P2_4);
}

//Inicializar interrupciones 
void setup_interrupts(void) {
    PCICR  |= (1 << PCIE2);
    PCMSK2 |= (1 << PCINT20) | (1 << PCINT19) | (1 << PCINT21);
}

//Habilitar el timer
void setup_timer1(void) {
    TCCR1A = 0;
    TCCR1B = 0;
    OCR1A  = 15624;
    TCCR1B |= (1 << WGM12);
    TIMSK1 |= (1 << OCIE1A);
}

//Inicializar el timer 
void timer1_start(void) {
    TCNT1 = 0;
    TCCR1B |= (1 << CS12) | (1 << CS10);
}

//Detener el timer
void timer1_stop(void) {
    TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
}

//Configurar que leds del display se encienden y cuales se apagan
void mostrarNumero(uint8_t num) {
    if (numeros[num][0]) PORTD |= (1 << DISP_A); else PORTD &= ~(1 << DISP_A);
    if (numeros[num][1]) PORTD |= (1 << DISP_B); else PORTD &= ~(1 << DISP_B);
    if (numeros[num][2]) PORTB |= (1 << DISP_C); else PORTB &= ~(1 << DISP_C);
    if (numeros[num][3]) PORTB |= (1 << DISP_D); else PORTB &= ~(1 << DISP_D);
    if (numeros[num][4]) PORTB |= (1 << DISP_E); else PORTB &= ~(1 << DISP_E);
    if (numeros[num][5]) PORTB |= (1 << DISP_F); else PORTB &= ~(1 << DISP_F);
    if (numeros[num][6]) PORTB |= (1 << DISP_G); else PORTB &= ~(1 << DISP_G);
}

//Inicializar leds para jugador 1
void leds_P1(void) {
    PORTC |= (1 << LED_P1_1) | (1 << LED_P1_2) |
             (1 << LED_P1_3) | (1 << LED_P1_4);
}

//Inicializar leds para jugador 2
void leds_P2(void) {
    PORTC |= (1 << LED_P2_1) | (1 << LED_P2_2);
    PORTB |= (1 << LED_P2_3);
    PORTD |= (1 << LED_P2_4);
}

//Incrementar un contador cuando el jugador 1 presione su boton
void incrementar_P1(void) {
    if (!iniciar_juego || ganador) return;

    if (++contador_P1 == 9) {
        ganador = 1;
        leds_P1();
    }
}

//Incrementar un contador cuando el jugador 2 presione su boton
void incrementar_P2(void) {
    if (!iniciar_juego || ganador) return;

    if (++contador_P2 == 9) {
        ganador = 2;
        leds_P2();
    }
}


ISR(PCINT2_vect) {
    static uint8_t estado_anterior = 0xFF;
    uint8_t estado_actual = PIND;

//si se presiona el boton para iniciar el contador, entonces inicia el timer 
    if ((estado_anterior & (1 << BTN)) && !(estado_actual & (1 << BTN))) { 
        if (!contando) {
            contador = 5;
            mostrarNumero(contador);
            contando = 1;
            ganador = 0; //Reinicia la variable del ganador para reiniciar el juego
            iniciar_juego = 0; // Reinicia la variable de iniciar juego para deshabilitar los push de los jugadores
            //Se reinicia el contador de cada jugador
			contador_P1 = 0;
            contador_P2 = 0;
            timer1_start();//Inician los 5 segundos del timer
        }
    }

//Si pasan los 5 segundos la variable iniciar juego se vuelve 1 y habilita los push de cada jugador 
    if (iniciar_juego) {
        if ((estado_anterior & (1 << BTN_P1)) && !(estado_actual & (1 << BTN_P1)))
            incrementar_P1();

        if ((estado_anterior & (1 << BTN_P2)) && !(estado_actual & (1 << BTN_P2)))
            incrementar_P2();
    }

    estado_anterior = estado_actual;
}

//Se utiliza una variable "contador" para saber cuando pasaron los 5 segundos y se detenda el timer y se inicie el juego
ISR(TIMER1_COMPA_vect) {
    if (contador > 0) {
        contador--;
        mostrarNumero(contador);
    } else {
        timer1_stop();
        contando = 0;
        iniciar_juego = 1;   
    }
}


//Se llaman a las funciones 
int main(void) {
    setup_pins();
    setup_interrupts();
    setup_timer1();

    mostrarNumero(contador);
    sei();

    while (1) {
    
    }
}
