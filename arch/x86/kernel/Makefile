ASFLAGS := $(KBUILD_CFLAGS)

# all: kernel

obj-y := head_32.o setup.o head32.o idt.o e820.o early_printk.o
obj-y := $(addprefix $(output_dir)/, $(obj-y))
# kernel: $(output_dir)/kernel.bin

build-in := $(wildcard $(output_dir)/*_build-in.o)

$(output_dir)/kernel.bin : $(output_dir)/kernel.elf
	$(OBJCOPY) -O binary $< $@

$(output_dir)/kernel.elf : $(obj-y) $(obj)/vmjnix.lds.S $(build-in)
	$(LD) -T $(obj)/vmjnix.lds.S -m elf_i386 -o $@ $(obj-y) $(build-in)

$(output_dir)/head_32.o : $(obj)/head_32.S
	$(CC) $(ASFLAGS) $< -o $@
