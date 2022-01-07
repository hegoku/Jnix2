
# b := $(addprefix $(makefile_path)/, $(addsuffix /Makefile, $(subdir)))
# include $(b)

export BITS

ifeq ($(CONFIG_X86_32),y)
	BITS := 32
else
	BITS := 64
endif

boot := arch/x86/boot
kernel := arch/x86/kernel
# mm := arch/x86/mm
# # lib := arch/x86/lib

core-y += arch/x86/mm/ arch/x86/entry/

# all : bzImage

#.PHONY = bzImage

bzImage: vmjnix
	$(MAKE) $(build)=$(boot)
	$(MAKE) $(build)=$(kernel)
	./build.out build/setup.bin build/bzimage
	cat $(output_dir)/kernel.bin >> build/bzimage



# $(SUBDIR):ECHO
# 	make -C $@
# ECHO:
# 	@echo $(SUBDIR)

# L_TARGET : main.o
# include $(abspath ./boot/Makefile)

# CORE_FILES += $(obj-y) $(L_TARGET)

# include $(TOPDIR)/Rules.make