ifeq ($(HEAP_SIZE),)
HEAP_SIZE      = 8388208
endif
ifeq ($(STACK_SIZE),)
STACK_SIZE     = 61800
endif

ifeq ($(PRODUCT),)
PRODUCT = TestCPP.pdx
endif

SELF_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

SDK = ${PLAYDATE_SDK_PATH}
ifeq ($(SDK),)
SDK = $(shell egrep '^\s*SDKRoot' ~/.Playdate/config | head -n 1 | cut -c9-)
endif

ifeq ($(SDK),)
$(error SDK path not found; set ENV value PLAYDATE_SDK_PATH)
endif

VPATH +=

# List C/C++ source files here
SRC += pdnewlib.c link.cpp main.cpp

# List all user directories here
UINCDIR +=

# List all user C/C++ define here, like -D_DEBUG=1
UDEFS +=

# Define ASM defines here
UADEFS +=

# List the user directory to look for the libraries here
ULIBDIR +=

# List all user libraries here
ULIBS +=

#CLANGFLAGS = -fsanitize=address

include buildsupport/common.mk