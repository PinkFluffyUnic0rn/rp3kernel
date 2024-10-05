#include "start.h"

#include "periph.h"

#define ARM_TIMER_CTL   (PBASE+0x0000B408)
#define ARM_TIMER_CNT   (PBASE+0x0000B420)

#define GPFSEL1         (PBASE+0x00200004)
#define GPSET0          (PBASE+0x0020001C)
#define GPCLR0          (PBASE+0x00200028)
#define GPPUD           (PBASE+0x00200094)
#define GPPUDCLK0       (PBASE+0x00200098)

#define AUX_ENABLES     (PBASE+0x00215004)
#define AUX_MU_IO_REG   (PBASE+0x00215040)
#define AUX_MU_IER_REG  (PBASE+0x00215044)
#define AUX_MU_IIR_REG  (PBASE+0x00215048)
#define AUX_MU_LCR_REG  (PBASE+0x0021504C)
#define AUX_MU_MCR_REG  (PBASE+0x00215050)
#define AUX_MU_LSR_REG  (PBASE+0x00215054)
#define AUX_MU_MSR_REG  (PBASE+0x00215058)
#define AUX_MU_SCRATCH  (PBASE+0x0021505C)
#define AUX_MU_CNTL_REG (PBASE+0x00215060)
#define AUX_MU_STAT_REG (PBASE+0x00215064)
#define AUX_MU_BAUD_REG (PBASE+0x00215068)

void PUTW(unsigned int, unsigned int);
unsigned int GETW(unsigned int);
void dummy(unsigned int);

unsigned int uart_lcr()
{
	return(GETW(AUX_MU_LSR_REG));
}

unsigned int uart_recv()
{
	while (1) {
		if (GETW(AUX_MU_LSR_REG) & 0x01)
			break;
	}

	return (GETW(AUX_MU_IO_REG) & 0xFF);
}

unsigned int uart_check()
{
	if (GETW(AUX_MU_LSR_REG) & 0x01)
		return(1);

	return(0);
}

unsigned int uart_overrun()
{
	if(GETW(AUX_MU_LSR_REG) & 0x02)
		return (1);

	return(0);
}

void uart_send(unsigned int c)
{
	while (1) {
		if (GETW(AUX_MU_LSR_REG) & 0x20)
			break;
	}
	
	PUTW(AUX_MU_IO_REG,c);
}

void uart_flush()
{
	while (1) {
		if ((GETW(AUX_MU_LSR_REG) & 0x100) == 0)
			break;
	}
}

void uart_init()
{
	unsigned int ra;

	ra = GETW(GPFSEL1);

	ra &= ~(7 << 12);	// gpio14
	ra |= 2 << 12;		// alt5
	ra &= ~(7 << 15);	// gpio15
	ra |= 2 << 15;		// alt5

	PUTW(GPFSEL1, ra);

	PUTW(GPPUD, 0);

	for (ra = 0; ra < 150; ra++)
		dummy(ra);

	PUTW(GPPUDCLK0, (1 << 14) | (1 << 15));

	for (ra = 0; ra < 150; ra++)
		dummy(ra);

	PUTW(GPPUDCLK0, 0);

	PUTW(AUX_ENABLES, 1);
	PUTW(AUX_MU_CNTL_REG, 0);	// disable UART receiver

	PUTW(AUX_MU_IER_REG, 0);	// clear FIFO
	PUTW(AUX_MU_LCR_REG, 3);	// 8-bit mode
	PUTW(AUX_MU_MCR_REG, 0);	// UART1_RTS to high
	PUTW(AUX_MU_IER_REG, 0);	// clear FIFO
	PUTW(AUX_MU_IIR_REG, 0xC6);	
	PUTW(AUX_MU_IIR_REG, 0);	
	PUTW(AUX_MU_BAUD_REG, 270);

	PUTW(AUX_MU_CNTL_REG, 3);	// enable UART receiver
}

void timer_init()
{
	PUTW(ARM_TIMER_CTL, 0x00F90000);
	PUTW(ARM_TIMER_CTL, 0x00F90200);
}

unsigned int timer_tick()
{
	return (GETW(ARM_TIMER_CNT));
}

void leds_off()
{

}

//-------------------------------------------------------------------------
//
// Copyright (c) 2012 David Welch dwelch@dwelch.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//-------------------------------------------------------------------------
