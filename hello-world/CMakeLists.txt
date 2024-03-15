add_executable(hello-world
	hello-world.c
)

pico_set_linker_script(hello-world $ENV{PICO_TOOLKIT_PATH}/src/picolibc-glue/pico-toolkit-flash.ld)

target_link_libraries(hello-world
	hardware_gpio
	hardware_uart
	cmsis_core
	pico_runtime
	pico_atomic
	pico_tls
	picolibc_glue
	pico_unique_id
)

pico_add_extra_outputs(hello-world)
