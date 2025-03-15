#!/bin/sh

set -e

ROOTFS_PATH="/persistence/rootfs.fat.$PLATFORM"
FILESYSTEM_PATH="/persistence/sdcard.img.$PLATFORM"

mount_rootfs() {
  mkdir -p rootfs/fs
  device=$(losetup --partscan --find --show $ROOTFS_PATH)
  mount ${device}p1 rootfs/fs
}

umount_rootfs() {
  umount rootfs/fs
  losetup -D
  rm -rf rootfs/fs
}

mount_filesystem() {
  rm -rf filesystem/fs/*
  mkdir -p filesystem/fs

  devname=$(losetup --partscan --find --show $FILESYSTEM_PATH)
  mount ${devname}p1 filesystem/fs 
}

umount_filesystem() {
  umount filesystem/fs
  losetup -D
}

build_usr()  {
  mkdir -p usr/build

  cd usr

  ./build.sh -r
  if [ $? -ne '0' ]; then 
    exit 1
  fi

  cd ..
}

deploy_usr() {
  mount_rootfs
  cp -r usr/out/* rootfs/fs
  cp -r usr/build/deploy/* rootfs/fs
  umount_rootfs
}

deploy_rootfs() {
  mount_rootfs
  mount_filesystem
  rm -rf filesystem/fs/*
  cp -rf rootfs/fs/* filesystem/fs/
  umount_filesystem
  umount_rootfs
}

deploy_uboot() {
    mkimage -f target/"$PLATFORM"_lvperf.its target/"$PLATFORM"_lvperf.itb
    mount_filesystem
    cp target/"$PLATFORM"_lvperf.itb filesystem/fs/"$PLATFORM".itb
    cp u-boot/uEnv.d/uEnv_$PLATFORM.txt filesystem/fs/uEnv.txt
    umount_filesystem
}

deploy () {
  deploy_usr
  deploy_rootfs
  deploy_uboot
}

build_usr
deploy

if [ "$QEMU_ARCH" = "aarch64" ]; then
    CPU="cortex-a72"
elif [ "$QEMU_ARCH" = "arm" ]; then
    CPU="cortex-a15"
else
    echo "Error: Unsupported architecture: $QEMU_ARCH" >&2
    echo "Supported architectures: arm, aarch64" >&2
    exit 1
fi

qemu-system-${QEMU_ARCH} \
  -semihosting \
  -smp 2 \
  -icount shift=0,sleep=on,align=on \
  -serial mon:stdio  \
  -M virt -cpu ${CPU} \
  -device virtio-blk-device,drive=hd0 \
  -drive if=none,file=$FILESYSTEM_PATH,id=hd0,format=raw,file.locking=off \
  -m 1024 \
  -kernel u-boot/u-boot \
  -nographic 
