ASFLAGS := $(KBUILD_CFLAGS)

# all: $(output_dir)/libmm.a

obj-y := extable.o page.o
obj-y := $(addprefix $(output_dir)/, $(obj-y))
# mm: $(obj-y)


# $(output_dir)/libmm.a: $(obj-y)
#	 $(AR) -rcs $@ $(obj-y)