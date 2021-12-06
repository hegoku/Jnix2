AS = i386-elf-as
CC = i386-elf-gcc
LD = i386-elf-ld
OBJCOPY = i386-elf-objcopy
CFLAGS := -I ../../../include -I ../include -c -nostdlib -fno-builtin -m16
ASFLAGS := -I ../../../include -I ../include -c -nostdlib -fno-builtin

ASFLAGS := $(KBUILD_CFLAGS)
KBUILD_CFLAGS := $(KBUILD_CFLAGS) -m16 #-mregparm=3

targets := $(output_dir)/setup.bin $(output_dir)/setup.elf

setup-y :=  bioscall.o regs.o \
	copy.o string_boot.o tty.o printf.o \
	video-mode.o video-bios.o video-vga.o \
	header.o boot_main.o memory.o pm.o pmjump.o
setup-y := $(addprefix $(output_dir)/, $(setup-y))

targets += $(setup-y)

# targets := $(addprefix $(output_dir)/, $(targets))

# boot-y := $(BUILD_DIR)/header.o $(BUILD_DIR)/bioscall.o $(BUILD_DIR)/copy.o $(BUILD_DIR)/main.o \
# 	$(BUILD_DIR)/string.o $(BUILD_DIR)/regs.o  $(BUILD_DIR)/tty.o $(BUILD_DIR)/printf.o \
# 	$(BUILD_DIR)/video-vga.o  $(BUILD_DIR)/video-bios.o $(BUILD_DIR)/video-mode.o \
# 	$(BUILD_DIR)/memory.o $(BUILD_DIR)/pm.o $(BUILD_DIR)/pmjump.o

$(output_dir)/setup.bin : $(output_dir)/setup.elf
	$(OBJCOPY) -O binary $< $@

$(output_dir)/setup.elf : $(setup-y) $(obj)/setup.ld
	$(LD) -s -T $(obj)/setup.ld -m elf_i386 -o $@ $(setup-y)

# $(output_dir)/%.o : %.c
# 	$(CC) $(CFLAGS) $< -o $@

# $(BUILD_DIR)/setup.elf : $(boot-y)
# 	$(LD) -s -T setup.ld -m elf_i386 -o $@ $(boot-y)

$(output_dir)/header.o : $(obj)/header.S
	$(CC) $(ASFLAGS) $< -o $@

$(output_dir)/bioscall.o : $(obj)/bioscall.S
	$(CC) $(ASFLAGS) $< -o $@

$(output_dir)/copy.o : $(obj)/copy.S
	$(CC) $(ASFLAGS) $< -o $@

# $(BUILD_DIR)/main.o : main.c
# 	$(CC) $(CFLAGS) $< -o $@

# $(BUILD_DIR)/printf.o : printf.c
# 	$(CC) $(CFLAGS) $< -o $@

# $(BUILD_DIR)/tty.o : tty.c
# 	$(CC) $(CFLAGS) $< -o $@

# $(BUILD_DIR)/regs.o : regs.c
# 	$(CC) $(CFLAGS) $< -o $@

# $(BUILD_DIR)/string.o : string.c
# 	$(CC) $(CFLAGS) $< -o $@

# $(BUILD_DIR)/video-vga.o : video-vga.c
# 	$(CC) $(CFLAGS) $< -o $@

# $(BUILD_DIR)/video-bios.o : video-bios.c
# 	$(CC) $(CFLAGS) $< -o $@

# $(BUILD_DIR)/video-mode.o : video-mode.c
# 	$(CC) $(CFLAGS) $< -o $@

# $(BUILD_DIR)/memory.o : memory.c
# 	$(CC) $(CFLAGS) $< -o $@

# $(BUILD_DIR)/pm.o : pm.c
# 	$(CC) $(CFLAGS) $< -o $@

$(output_dir)/pmjump.o : $(obj)/pmjump.S
	$(CC) $(ASFLAGS) $< -o $@

# include $(TOPDIR)/Rules.make

clean:
	rm $(output_dir)/*.*