#!/bin/sh

if [ ! -f installation_done ]; then
  echo "Running first install"
  ./docker/do_first_install.sh
fi

mkdir -p usr/build
cd usr; ./build.sh -r; cd ..

mkdir rootfs/fs

mount $(losetup --partscan --find --show ./rootfs/rootfs.fat)p1 rootfs/fs
mount $(losetup --partscan --find --show ./filesystem/sdcard.img.virt32)p1 filesystem/fs

cp -r usr/out/* rootfs/fs
cp -r usr/build/deploy/* rootfs/fs
cp -rf rootfs/fs/* filesystem/fs/

umount rootfs/fs
umount filesystem/fs
losetup -D
rm -rf rootfs/fs

qemu-system-arm \
  -semihosting \
  -smp 4 \
  -serial mon:stdio  \
  -M virt   -cpu cortex-a15 \
  -device virtio-blk-device,drive=hd0 \
  -drive if=none,file=filesystem/sdcard.img.virt32,id=hd0,format=raw,file.locking=off \
  -m 1024 \
  -kernel u-boot/u-boot \
  -nographic 
