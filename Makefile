ARCH = i386
MAKE = make

# Additional ARCH settings for x86
ifeq ($(ARCH),i386)
        SRCARCH := x86
endif
ifeq ($(ARCH),x86_64)
        SRCARCH := x86
endif

KCONFIG_CONFIG :=

export ARCH MAKE SRCARCH KCONFIG_CONFIG

this-makefile := $(lastword $(MAKEFILE_LIST))
abs_srctree := $(realpath $(dir $(this-makefile)))
export abs_srctree abs_objtree

srctree := $(abs_srctree)
export srctree

output_dir = $(srctree)/build
build := -f $(srctree)/Makefile.build output_dir="$(output_dir)" obj
export build

include $(srctree)/arch/$(SRCARCH)/Makefile

core-y += mm/ init/ lib/ kernel/ fs/
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
	x86_64-elf-objdump -l -d -r build/kernel.elf >setup.txt
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
	qemu-system-x86_64 -s -m 1G  -kernel build/bzimage

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
