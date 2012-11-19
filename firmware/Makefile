PROJECT=template


LSCRIPT=core/stm32f4xx_flash.ld

OPTIMIZATION = -O2

#########################################################################

SRC=$(wildcard  *.c libs/*.c) \
	core/syscalls.c core/stm32fxxx_it.c core/system_stm32f4xx.c \
	../animations/plasma_bigsin.c


ASRC=core/startup_stm32f4xx.s
OBJECTS= $(SRC:.c=.o) $(ASRC:.s=.o)
LSTFILES= $(SRC:.c=.lst)
HEADERS=$(wildcard core/*.h *.h)

#  Compiler Options
GCFLAGS = -DSTM32F=4 -ffreestanding -std=gnu99 -mcpu=cortex-m4 -mthumb $(OPTIMIZATION) -I. -Icore -DARM_MATH_CM4 -DUSE_STDPERIPH_DRIVER 
GCFLAGS+= -Dlun1k=1 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -falign-functions=16 
GCFLAGS+=-ISTM32_USB_Device_Library/Class/cdc/inc
GCFLAGS+=-ISTM32_USB_OTG_Driver/inc
GCFLAGS+=-ISTM32_USB_Device_Library/Core/inc
# Warnings
GCFLAGS += -Wno-strict-aliasing -Wstrict-prototypes -Wundef -Wall -Wextra -Wunreachable-code  
# Optimizazions
GCFLAGS += -fstrict-aliasing -fsingle-precision-constant -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fno-builtin -ffunction-sections -fno-common -fdata-sections 
# Debug stuff
GCFLAGS += -Wa,-adhlns=$(<:.c=.lst),-gstabs -g 

GCFLAGS+= -ISTM32F4_drivers/inc


LDFLAGS = -mcpu=cortex-m4 -mthumb $(OPTIMIZATION) -T$(LSCRIPT) 
LDFLAGS+= -mfpu=fpv4-sp-d16 -mfloat-abi=hard -falign-functions=16
LDFLAGS+= -LSTM32F4_drivers/build -lSTM32F4xx_drivers -lm -lnosys -lc


#  Compiler/Assembler Paths
GCC = arm-none-eabi-gcc
AS = arm-none-eabi-as
OBJCOPY = arm-none-eabi-objcopy
REMOVE = rm -f
SIZE = arm-none-eabi-size

#########################################################################

all: STM32F4_drivers/build/libSTM32F4_drivers.a $(PROJECT).bin Makefile 
	@$(SIZE) $(PROJECT).elf -A | grep 'text\|data\|section\|bss\|ccm'

STM32F4_drivers/build/libSTM32F4_drivers.a:
	@make -C STM32F4_drivers/build

$(PROJECT).bin: $(PROJECT).elf Makefile
	@echo "generating $(PROJECT).bin"
	@$(OBJCOPY)  -g -S -R .stack -O binary $(PROJECT).elf $(PROJECT).bin

$(PROJECT).elf: $(OBJECTS) Makefile $(LSCRIPT)
	@echo "  LD $(PROJECT).elf"
	@$(GCC) $(OBJECTS) $(LDFLAGS)  -o $(PROJECT).elf

clean:
	$(REMOVE) $(OBJECTS)
	$(REMOVE) $(LSTFILES)
	$(REMOVE) $(PROJECT).bin
	$(REMOVE) $(PROJECT).elf

#########################################################################

%.o: %.c Makefile $(HEADERS)
	@echo "  GCC $<"
	@$(GCC) $(GCFLAGS) -o $@ -c $<

%.o: %.s Makefile 
	@echo "  AS $<"
	@$(AS) $(ASFLAGS) -o $@  $< 

#########################################################################

flash: all
	dfu-util -a 0 -s 0x08000000 -D $(PROJECT).bin -R

.PHONY : clean all flash
