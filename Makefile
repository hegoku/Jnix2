ARCH = x86
MAKE = make

export ARCH

this-makefile := $(lastword $(MAKEFILE_LIST))
abs_srctree := $(realpath $(dir $(this-makefile)))
export abs_srctree abs_objtree

srctree := $(abs_srctree)
export srctree

output_dir = $(srctree)/build
build := -f $(srctree)/Makefile.build output_dir="$(output_dir)" obj

include $(srctree)/arch/$(ARCH)/Makefile

core-y += mm/ init/ lib/ kernel/
core-obj := $(addsuffix /%.o,$(core-y))

vmlinux-dirs := $(patsubst %/,%,$(filter %/, \
	$(core-y) $(core-m) $(drivers-y) $(drivers-m) \
	$(libs-y) $(libs-m)))
	
build-dirs	:= $(vmlinux-dirs)

vmjnix: $(build-dirs)
	for dir in $(build-dirs); do $(MAKE) $(build)=$$dir; done 
#	$(MAKE) $(build)=$<
#	$(MAKE) output_dir="$(BUILD_DIR)" -C mm

debug:
	i386-elf-objdump -l -d -r build/kernel.elf >setup.txt
	#i386-elf-objdump  -m i8086 -l -d -r build/kernel.elf >setup.txt
	
buildimage:
	hdiutil attach -imagekey diskimage-class=CRawDiskImage c1.img
	cp build/bzimage /Volumes/NO\ NAME/bzimage
	hdiutil detach /Volumes/NO\ NAME/
	bochs
	#qemu-system-i386 -s c1.img

clean:
	rm build/*

run:
	qemu-system-i386 -s -m 4G  -kernel build/bzimage

map:
	i386-elf-nm -n build/kernel.elf | grep -v '\( [aNUw] \)\|\(__crc_\)\|\( \$[adt]\)\|\( .L\)' > System.map
#CORE_FILES :=
# SUBDIR := $(TOPDIR)/arch/$(ARCH)/

# all: $(SUBDIR)

# $(SUBDIR):ECHO
# 	make -C $@
# ECHO:
# 	@echo $(SUBDIR)
#image: CORE_FILES
#	$(LD) -s -Tjnix.ls.S -m elf_i386 -o kernel.bin $(CORE_FILES)
