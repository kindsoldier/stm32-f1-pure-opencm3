# Author, Copyright: Oleg Borodin <onborodin@gmail.com> 2018

.SECONDARY:

CFLAGS+= -I. -Os -mthumb -mcpu=cortex-m3 -march=armv7-m -msoft-float -DSTM32F1 -std=c99
CFLAGS+= -fno-common -ffunction-sections -fdata-sections
CFLAGS+= -g -gdwarf-2
#CFLAGS+= -save-temps

LDFLAGS+= ${CFLAGS}
LDFLAGS+= --static
LDFLAGS+= -nostartfiles
LDFLAGS+= -T master.ld

LDFLAGS+= -Wl,-Map=master.map
LDFLAGS+= -Wl,--cref -Wl,--gc-sections
LDFLAGS+= -lopencm3_stm32f1
#LDFLAGS+= -lc -lgcc
LDFLAGS+= -Wl,--start-group -lc -lgcc -lm -lnosys -Wl,--end-group

all: master.bin

MASTER_OBJS+= ds3231.o
MASTER_OBJS+= master.o 
MASTER_OBJS+= syscall.o
MASTER_OBJS+= buffer.o
MASTER_OBJS+= uastdio.o
MASTER_OBJS+= datetime.o
MASTER_OBJS+= st7735.o
MASTER_OBJS+= console.o
MASTER_OBJS+= mpu6050.o
MASTER_OBJS+= i2creg.o

master.elf: $(MASTER_OBJS)
	arm-eabi-gcc $(^F) $(LDFLAGS) -o $@ 
	arm-eabi-size --format=berkeley $@

%.o: %.c
	arm-eabi-gcc $(CFLAGS) -c -o $@ $<

%.o: %.S
	arm-eabi-as $(ASFLAGS) -o $@ $<

%.bin: %.elf
	arm-eabi-objcopy -O binary $< $@

%.elf: %.o
	arm-eabi-gcc $(^F) $(LDFLAGS) -o $@ 
	arm-eabi-size --format=berkeley $@

clean:
	rm -f *.i *.o *.elf *.bin *.map *~ *.hex *.d *.s

upload: master.upl

%.upl: %.bin
	@openocd \
	    -c 'puts "--- START --------------------"' \
	    -f 'interface/stlink-v2.cfg' \
	    -f 'target/stm32f1x.cfg'  \
	    -c 'puts "--- INIT --------------------"' \
	    -c "init" \
	    -c "reset halt" \
	    -c 'puts "--- WRITE --------------------"' \
	    -c "flash write_image erase $< 0x08000000"\
	    -c 'puts "--- VERIFY --------------------"' \
	    -c "verify_image $<" \
	    -c 'puts "--- RESET --------------------"' \
	    -c "reset" \
	    -c 'puts "--- DONE --------------------"' \
	    -c "shutdown"

#EOF
