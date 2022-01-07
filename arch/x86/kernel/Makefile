ASFLAGS := $(KBUILD_CFLAGS)

# all: kernel

kernel-y := head_$(BITS).so setup.o head$(BITS).o idt.o e820.o early_printk.o fault.o time.o i8259.o irqinit.o traps.o irq.o irq_$(BITS).o \
	i8253.o process.o common.o process_$(BITS).o
kernel-y := $(addprefix $(output_dir)/, $(kernel-y))
# kernel: $(output_dir)/kernel.bin

build-in := $(wildcard $(output_dir)/*_build-in.o)

$(output_dir)/kernel.bin : $(output_dir)/kernel.elf
	$(OBJCOPY) -O binary $< $@

$(output_dir)/kernel.elf : $(kernel-y) $(obj)/vmjnix.lds.S $(build-in)
	$(LD) -T $(obj)/vmjnix.lds.S -m elf_i386 -o $@ $(kernel-y) $(build-in)

#$(output_dir)/head_32.o : $(obj)/head_32.S
#	$(CC) $(ASFLAGS) $< -o $@
