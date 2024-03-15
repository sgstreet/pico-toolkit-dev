add_executable(picolibc-tls-test
	picolibc-tls-test.c
)

pico_set_linker_script(picolibc-tls-test ${CMAKE_CURRENT_SOURCE_DIR}/pico-toolkit-flash.ld)

# pull in common dependencies
target_link_libraries(picolibc-tls-test
	hardware_gpio
	hardware_uart
	cmsis_core
	pico_runtime
	pico_time
	pico_unique_id
	pico_atomic
	pico_sbrk
	picolibc_glue
	pico_multicore
)

# create map/bin/hex file etc.
pico_add_extra_outputs(picolibc-tls-test)
