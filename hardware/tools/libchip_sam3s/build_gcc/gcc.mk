
# Tool suffix when cross-compiling
#CROSS_COMPILE = ../../CodeSourcery_arm/bin/arm-none-eabi-
CROSS_COMPILE = C:/CodeSourcery_2011.03-42/bin/arm-none-eabi-

# Compilation tools
AR = $(CROSS_COMPILE)ar
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
#LD = $(CROSS_COMPILE)ld
#SIZE = $(CROSS_COMPILE)size
NM = $(CROSS_COMPILE)nm
#OBJCOPY = $(CROSS_COMPILE)objcopy
RM=cs-rm -Rf
SEP=/

# Flags

CFLAGS += -Wall -Wchar-subscripts -Wcomment -Wformat=2 -Wimplicit-int
CFLAGS += -Werror-implicit-function-declaration -Wmain -Wparentheses
CFLAGS += -Wsequence-point -Wreturn-type -Wswitch -Wtrigraphs -Wunused
CFLAGS += -Wuninitialized -Wunknown-pragmas -Wfloat-equal -Wundef
CFLAGS += -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings
CFLAGS += -Wsign-compare -Waggregate-return -Wstrict-prototypes
CFLAGS += -Wmissing-prototypes -Wmissing-declarations
CFLAGS += -Wformat -Wmissing-format-attribute -Wno-deprecated-declarations
CFLAGS += -Wpacked -Wredundant-decls -Wnested-externs -Winline -Wlong-long
CFLAGS += -Wunreachable-code
CFLAGS += -Wcast-align
#CFLAGS += -Wmissing-noreturn
#CFLAGS += -Wconversion

# To reduce application size use only integer printf function.
CFLAGS += -Dprintf=iprintf

CFLAGS += --param max-inline-insns-single=500 -mcpu=cortex-m3 -mthumb -mlong-calls -ffunction-sections
CFLAGS += $(OPTIMIZATION) $(INCLUDES) -D$(CHIP)

ASFLAGS = -mcpu=cortex-m3 -mthumb -Wall -a -g $(INCLUDES)
