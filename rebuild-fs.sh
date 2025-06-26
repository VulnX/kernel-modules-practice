#!/bin/bash
cd initramfs
find . | cpio -H newc -o | gzip > ../initramfs.cpio.gz
cd ..