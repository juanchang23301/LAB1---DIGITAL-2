#include "DISP_7SEG.h"

// Tabla de segmentos
static const uint8_t numeros[6][7] = {
	{1,1,1,1,1,1,0}, // 0
	{0,1,1,0,0,0,0}, // 1
	{1,1,0,1,1,0,1}, // 2
	{1,1,1,1,0,0,1}, // 3
	{0,1,1,0,0,1,1}, // 4
	{1,0,1,1,0,1,1}  // 5
};

//Inicializar pines del diplay
void    disp_7seg_init(void) {
	DDRD |= (1 << DISP_A) | (1 << DISP_B);
	DDRB |= (1 << DISP_C) | (1 << DISP_D) |
	(1 << DISP_E) | (1 << DISP_F) |
	(1 << DISP_G);
}

//Configurar que leds del display se encienden y cuales se apagan
void disp_7seg_show(uint8_t num) {
	if (num > 5) return;

	if (numeros[num][0]) PORTD |= (1 << DISP_A); else PORTD &= ~(1 << DISP_A);
	if (numeros[num][1]) PORTD |= (1 << DISP_B); else PORTD &= ~(1 << DISP_B);
	if (numeros[num][2]) PORTB |= (1 << DISP_C); else PORTB &= ~(1 << DISP_C);
	if (numeros[num][3]) PORTB |= (1 << DISP_D); else PORTB &= ~(1 << DISP_D);
	if (numeros[num][4]) PORTB |= (1 << DISP_E); else PORTB &= ~(1 << DISP_E);
	if (numeros[num][5]) PORTB |= (1 << DISP_F); else PORTB &= ~(1 << DISP_F);
	if (numeros[num][6]) PORTB |= (1 << DISP_G); else PORTB &= ~(1 << DISP_G);
}
