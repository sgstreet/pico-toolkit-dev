/*
 * hello-world.c
 *
 *  Created on: Mar 13, 2024
 *      Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#include <stdio.h>
#include <stdint.h>

#include <RP2040.h>

#include <hardware/gpio.h>
#include <hardware/uart.h>

#if 0
#include <pico/iob.h>
#endif

#include <pico/unique_id.h>

#define UART_ID uart0
#define BAUD_RATE 115200

#define UART_TX_PIN 0
#define UART_RX_PIN 1

#if 0
int picolibc_putc(char c, FILE *file);
int picolibc_getc(FILE *file);

int picolibc_putc(char c, FILE *file)
{
	uart_putc(UART_ID, c);
	return c;
}

int picolibc_getc(FILE *file)
{
	return uart_getc(UART_ID);
}
#endif

static __attribute__((constructor)) void console_init(void)
{
	/* Set up our UART with the required speed. */
	uart_init(UART_ID, BAUD_RATE);

	/*
	 * Set the TX and RX pins by using the function select on the GPIO
	 * Set datasheet for more information on function select
	 */
	gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
	gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
}

static uint64_t board_id(void)
{
	pico_unique_board_id_t board_id;
	pico_get_unique_board_id(&board_id);
	return *(uint64_t *)(board_id.id);
}

int main()
{

	/* Send out a character without any conversions */
	putc('A', stdout);

	/* Send out a character but do CR/LF conversions */
	putc('B', stdout);

	/* Send out a string, with CR/LF conversions */
	puts(" Hello, PICOLIBC!");

	/* Print also */
	uint32_t clock = SystemCoreClock;
	uint64_t id = board_id();
	printf("Hello from board 0x%llx running at %uHz\n", id, clock);
}
