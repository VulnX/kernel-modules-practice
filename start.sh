#!/bin/bash

set -e

# 1. Build the kernel module
echo "Building the kernel module..."
cd modules
make
cd ..

# 2. Move the kernel modules and the test binary into the initramfs directory
echo "Moving modules and test binary to initramfs directory..."
mv modules/*.ko initramfs/
mv modules/test initramfs/

# 3. Rebuild the initramfs
echo "Rebuilding initramfs..."
./rebuild-fs.sh


# 5. Start QEMU with the correct kernel and initramfs
echo "Starting QEMU with the kernel and initramfs..."
qemu-system-x86_64 \
  -kernel ./bzImage \
  -cpu qemu64,+smep,+smap,+rdrand \
  -initrd ./initramfs.cpio.gz \
  -append "console=ttyS0" \
  -nographic
