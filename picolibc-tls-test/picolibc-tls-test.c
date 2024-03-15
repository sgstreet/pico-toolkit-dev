/*
 * hello-world.c
 *
 *  Created on: Mar 13, 2024
 *      Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdatomic.h>
#include <unistd.h>

#include <picotls.h>

#include <pico/time.h>

#include <RP2040.h>

#include <hardware/sync.h>
#include <hardware/gpio.h>
#include <hardware/uart.h>

#include <pico/multicore.h>
#include <pico/unique_id.h>

#define UART_ID uart0
#define BAUD_RATE 115200

#define UART_TX_PIN 0
#define UART_RX_PIN 1

int picolibc_putc(char c, FILE *file);
int picolibc_flush(FILE *file);

#ifndef thread_local
#define thread_local _Thread_local
#endif

thread_local char io_buffer[128] = "deadbeef";
thread_local char *io_ptr = 0;
thread_local int core_counter = 0;
atomic_uint lock = 0;
thread_local uint32_t tls_marker = 0xdeadbeef;

int picolibc_putc(char c, FILE *file)
{
	if (tls_marker != 0xdeadbeef)
		abort();

	if (!io_ptr)
		io_ptr = io_buffer;

	*io_ptr++ = c;

	if ((io_ptr - io_buffer) >= sizeof(io_buffer) || c == '\n')
		picolibc_flush(file);

	return c;
}

int picolibc_flush(FILE *file)
{
	uint expected = 0;
	while (!atomic_compare_exchange_strong(&lock, &expected, get_core_num() + 1)) {
		__wfe();
		expected = 0;
	}

	char *current = io_buffer;
	while (current < io_ptr) {
		uart_putc_raw(UART_ID, *current);
		if (*current++ == '\n')
			uart_putc_raw(UART_ID, '\r');
	}

	io_ptr = io_buffer;

	atomic_store(&lock, 0);
	__sev();

	return 0;
}

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

void hello(void)
{
	if (tls_marker != 0xdeadbeef)
		abort();

	printf("%d Hello from board 0x%llx running at %uHz on core %u\n", core_counter++, board_id(), SystemCoreClock, get_core_num());
}

static void _tls_core_init(void)
{
	static void *tls_block[NUM_CORES] = { 0 };

	extern __attribute__((weak)) char __core_0_tls[];
	extern __attribute__((weak)) char __core_1_tls[];

	uint32_t core = get_core_num();

	/* If the linker did not provide a tls block for this core */
	if (!tls_block[core]) {
		tls_block[core] = core ? __core_1_tls : __core_0_tls;
		if (!tls_block[core])
			tls_block[core] = sbrk(_tls_size());
	}

	/* Initialize the block */
	_init_tls(tls_block[core]);

	/* Set the block */
	_set_tls(tls_block[core]);
}
__attribute__((section(".preinit_array"))) void *preinit_tls_core_init = _tls_core_init;

static void core_1_thread(void)
{
	_tls_core_init();

	if (tls_marker != 0xdeadbeef)
		abort();

	while (true) {
		hello();
		sleep_ms(250);
	}
}

int main()
{
	if (tls_marker != 0xdeadbeef)
		abort();

	multicore_launch_core1(core_1_thread);

	while (true) {
		hello();
		sleep_ms(500);
	}
}
