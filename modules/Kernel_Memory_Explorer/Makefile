# Define the object and module files
obj-m += kmem_explorer.o

# The directory for the kernel build (dynamically determined)
KDIR := ~/Downloads/linux
PWD := $(shell pwd)

# Additional flags
EXTRA_CFLAGS += -I$(KDIR)/include
KBUILD_CFLAGS += -fno-strict-aliasing

# Default build target (all)
all: test modules

# Rule to build kernel modules
modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Rule to build the test binary (static C program)
test:
	gcc -static -o test_kmem_explorer test_kmem_explorer.c

# Clean target to remove build artifacts
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f test_kmem_explorer