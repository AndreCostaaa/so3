#!/bin/sh

cd filesystem; ./create_img.sh virt32 && cd ..
cd rootfs; ./create_ramfs.sh && cd ..

mkimage -f target/virt32_lvperf.its target/virt32_lvperf.itb;

mount $(losetup --partscan --find --show filesystem/sdcard.img.virt32)p1 filesystem/fs

cp target/virt32_lvperf.itb filesystem/fs
cp u-boot/uEnv.d/uEnv_virt32.txt filesystem/fs/uEnv.txt

umount filesystem/fs
losetup -D

echo "OK" > installation_done
