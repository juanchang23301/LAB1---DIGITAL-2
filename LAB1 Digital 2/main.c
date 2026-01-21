/************************************************************************
 Contador automático 5 a 0 con pushbutton + TIMER
 ATmega328P / Arduino Nano
************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

// Pines del display
#define DISP_A PD6
#define DISP_B PD7
#define DISP_C PB0
#define DISP_D PB1
#define DISP_E PB2
#define DISP_F PB3
#define DISP_G PB4

// Botón
#define BTN PD4   // PCINT20

volatile uint8_t contador = 5;
volatile uint8_t contando = 0;

// Tabla de segmentos (a b c d e f g)
const uint8_t numeros[6][7] = {
    {1,1,1,1,1,1,0}, // 0
    {0,1,1,0,0,0,0}, // 1
    {1,1,0,1,1,0,1}, // 2
    {1,1,1,1,0,0,1}, // 3
    {0,1,1,0,0,1,1}, // 4
    {1,0,1,1,0,1,1}  // 5
};

void setup_pins(void) {
    // Display como salida
    DDRD |= (1 << DISP_A) | (1 << DISP_B);
    DDRB |= (1 << DISP_C) | (1 << DISP_D) |
            (1 << DISP_E) | (1 << DISP_F) |
            (1 << DISP_G);

    // Botón como entrada con pull-up
    DDRD &= ~(1 << BTN);
    PORTD |= (1 << BTN);
}

void setup_interrupts(void) {
    PCICR |= (1 << PCIE2);          // Habilita PCINT en PORTD
    PCMSK2 |= (1 << PCINT20);       // PD4
}

void setup_timer1(void) {
    TCCR1A = 0;
    TCCR1B = 0;

    OCR1A = 15624;                 
    TCCR1B |= (1 << WGM12);        
    TIMSK1 |= (1 << OCIE1A);      
}

void timer1_start(void) {
    TCNT1 = 0;
    TCCR1B |= (1 << CS12) | (1 << CS10); // Prescaler 1024
}

void timer1_stop(void) {
    TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
}


void mostrarNumero(uint8_t num) {
    if (numeros[num][0]) PORTD |= (1 << DISP_A); else PORTD &= ~(1 << DISP_A);
    if (numeros[num][1]) PORTD |= (1 << DISP_B); else PORTD &= ~(1 << DISP_B);
    if (numeros[num][2]) PORTB |= (1 << DISP_C); else PORTB &= ~(1 << DISP_C);
    if (numeros[num][3]) PORTB |= (1 << DISP_D); else PORTB &= ~(1 << DISP_D);
    if (numeros[num][4]) PORTB |= (1 << DISP_E); else PORTB &= ~(1 << DISP_E);
    if (numeros[num][5]) PORTB |= (1 << DISP_F); else PORTB &= ~(1 << DISP_F);
    if (numeros[num][6]) PORTB |= (1 << DISP_G); else PORTB &= ~(1 << DISP_G);
}


ISR(PCINT2_vect) {
    if (!(PIND & (1 << BTN))) { 
        if (!contando) {
            contador = 5;
            mostrarNumero(contador);
            contando = 1;
            timer1_start();
        }
    }
}

ISR(TIMER1_COMPA_vect) {
    if (contador > 0) {
        contador--;
        mostrarNumero(contador);
    } else {
        timer1_stop();
        contando = 0;
    }
}

int main(void) {
    setup_pins();
    setup_interrupts();
    setup_timer1();

    mostrarNumero(contador);

    sei();

    while (1) {
    }
}
