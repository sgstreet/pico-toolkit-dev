.DEFAULT_GOAL := all
ifeq (${MAKECMDGOALS},)
MAKECMDGOALS := all
endif

export BUILD_TYPE ?= debug
export PROJECT_ROOT ?= ${CURDIR}
export OUTPUT_ROOT ?= ${PROJECT_ROOT}/build
export TOOLS_ROOT ?= ${PROJECT_ROOT}/tools
export PREFIX ?= ${PROJECT_ROOT}/rootfs

export INSTALL_ROOT := ${PREFIX}$(if ${BOARD_TYPE},/${BOARD_TYPE},)$(if ${BUILD_TYPE},/${BUILD_TYPE},)
export BUILD_ROOT := ${OUTPUT_ROOT}$(if ${BOARD_TYPE},/${BOARD_TYPE},)$(if ${BUILD_TYPE},/${BUILD_TYPE},)

ifeq (${V},)
SILENT=--silent
endif

export PICO_SDK_PATH ?= ${PROJECT_ROOT}/pico-sdk
export PICO_EXTRAS_PATH ?= ${PROJECT_ROOT}/pico-extras
export PICO_TOOLKIT_PATH ?= ${PROJECT_ROOT}/pico-toolkit
#export PICOLIBC_PATH ?= ${PREFIX}/arm-none-eabi

MAKEFLAGS += ${SILENT}

${OUTPUT_ROOT}/.submodule-init:
	+[ -d $(dir $@) ] || mkdir -p $(dir $@)
	git submodule update --init --recursive
	touch $@

${PROJECT_ROOT}/CMakeLists.txt: ${OUTPUT_ROOT}/.submodule-init
 
${BUILD_ROOT}/Makefile: ${PROJECT_ROOT}/CMakeLists.txt
	+[ -d $(dir $@) ] || mkdir -p $(dir $@)
	cmake -B${BUILD_ROOT} -S${PROJECT_ROOT} -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DPICO_TOOLKIT_TESTS_ENABLED=true

all: ${BUILD_ROOT}/Makefile  
	@echo "BUILD pico-toolkit"
	cd ${BUILD_ROOT} && make
	
clean: ${BUILD_ROOT}/Makefile
	@echo "CLEAN pico-toolkit"
	cd ${BUILD_ROOT} && make clean

distclean:
	@echo "DISTCLEAN ${BUILD_ROOT} ${INSTALL_ROOT}"
	-${RM} -r ${BUILD_ROOT} ${INSTALL_ROOT}

realclean:
	@echo "REALCLEAN ${OUTPUT_ROOT} ${PREFIX} ${PROJECT_ROOT}/tmp"
	git submodule deinit --all
	-${RM} -r ${OUTPUT_ROOT} ${PREFIX} ${PROJECT_ROOT}/tmp

info:
	@echo "PICO_SDK_PATH=${PICO_SDK_PATH}"
	@echo "PICO_TOOLKIT_PATH=${PICO_TOOLKIT_PATH}"
	@echo "PICOLIBC_PATH=${PICOLIBC_PATH}"
	@echo "BOARD_TYPE=${BOARD_TYPE}"
	@echo "BUILD_TYPE=${BUILD_TYPE}"
	@echo "PROJECT_ROOT=${PROJECT_ROOT}"
	@echo "OUTPUT_ROOT=${OUTPUT_ROOT}"
	@echo "TOOLS_ROOT=${TOOLS_ROOT}"
	@echo "PREFIX=${PREFIX}"
	@echo "BUILD_ROOT=${BUILD_ROOT}"
	@echo "INSTALL_ROOT=${INSTALL_ROOT}"
.PHONY: info

help:
	@echo "all           Build everything"
	@echo "setup         Setup host build tools"
	@echo "clean         Run the clean action of all projects"
	@echo "distclean     Delete all build artifacts and qualified directories"
	@echo "realclean     Delete all build artifacts, directories and external repositories"
	@echo "info          Display build configuration"
.PHONY: help

${V}.SILENT:
