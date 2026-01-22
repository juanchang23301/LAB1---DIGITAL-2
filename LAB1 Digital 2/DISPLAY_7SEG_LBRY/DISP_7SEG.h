#ifndef DISP_7SEG_H
#define DISP_7SEG_H

#include <avr/io.h>

// Display
#define DISP_A PD6
#define DISP_B PD7
#define DISP_C PB0
#define DISP_D PB1
#define DISP_E PB2
#define DISP_F PB3
#define DISP_G PB4

//Inicializar pines del diplay
void    disp_7seg_init(void);

//Configurar que leds del display se encienden y cuales se apagan
void    disp_7seg_show(uint8_t num);

#endif
