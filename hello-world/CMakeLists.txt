add_executable(hello-world
	hello-world.c
)

#pico_set_linker_script(hello-world ${CMAKE_CURRENT_SOURCE_DIR}/pico-toolkit-flash.ld)

# pull in common dependencies
target_link_libraries(hello-world
	hardware_gpio
	hardware_uart
	cmsis_core
	pico_runtime
#	pico_atomic
#	pico_tls
#	picolibc_glue
	pico_unique_id
)

# create map/bin/hex file etc.
pico_add_extra_outputs(hello-world)
