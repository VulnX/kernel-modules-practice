#!/bin/bash

set -e

# 1. Build the kernel module
echo "Building the kernel module..."
cd modules
make
cd ..

# 2. Move the kernel modules and the test binaries into the initramfs directory
echo "Moving modules and test binary to initramfs directory..."
cp modules/*/*.ko initramfs/modules
for test_file in $(find modules -name "test_*" | grep -v "\..*$") ; do mv $test_file initramfs/tests ; done

# 3. Rebuild the initramfs
echo "Rebuilding initramfs..."
./rebuild-fs.sh

# 4. Cleanup initramfs of unused files
rm -rf initramfs/modules/*
rm -rf initramfs/tests/*

# 5. Start QEMU with the correct kernel and initramfs
echo "Starting QEMU with the kernel and initramfs..."
qemu-system-x86_64 \
  -kernel ./bzImage \
  -cpu qemu64,+smep,+smap,+rdrand \
  -initrd ./initramfs.cpio.gz \
  -append "console=ttyS0" \
  -nographic