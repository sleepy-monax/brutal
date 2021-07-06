.SUFFIXES:
.DEFAULT_GOAL := all
export LC_ALL=C

CFLAGS_STD ?= \
	-std=gnu2x

CFLAGS_OPT ?= \
	-g

CFLAGS_WARN ?= \
	-Wall \
	-Wextra \
	-Werror

INC_LIBC= \
	-Isources/libc/ansi \
	-Isources/libc/posix \
	-Isources/libc/bsd \
	-Isources/libc/gnu

INC_LIBBRUTAL= \
	-Isources/libc/

HOST_CFLAGS_INC ?= \
	$(INC_LIBBRUTAL) \
	-Isources/ \
	-Ithirdparty

CROSS_CFLAGS_INC ?= \
	$(INC_LIBC) \
	$(INC_LIBBRUTAL) \
	-Isources/ \
	-Ithirdparty

BUILDDIR=build/$(CONFIG_ARCH)-$(CONFIG_TOOLCHAIN)
BUILDDIR_HOST=$(BUILDDIR)/host
BUILDDIR_CROSS=$(BUILDDIR)/cross

MKCWD=mkdir -p $(@D)

include meta/config/default.mk
include meta/toolchain/$(CONFIG_TOOLCHAIN)/.build.mk

include sources/arch/.build.mk
include sources/kernel/.build.mk
include sources/libc/.build.mk
include sources/sysroot/.build.mk
include sources/test/.build.mk
include sources/apps/.build.mk

all: $(TARGETS)

run: $(SYSROOT_ISO)
	qemu-system-x86_64 \
		-M q35 \
		-cpu host \
		-smp 4 \
		-m 256M \
		-enable-kvm \
		-serial mon:stdio \
		-cdrom $(SYSROOT_ISO)

bochs:
	bochs

clean:
	rm -rf build/

-include $(DEPENDENCIES)
