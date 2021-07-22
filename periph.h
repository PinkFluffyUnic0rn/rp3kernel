#ifndef PERIPH_H
#define PERIPH_H

void uart_init();

unsigned int uart_lcr();

void uart_flush();

void uart_send(unsigned int);

unsigned int uart_recv();

unsigned int uart_check();

unsigned int uart_overrun();

void timer_init();

unsigned int timer_tick();

void leds_off();

#endif
