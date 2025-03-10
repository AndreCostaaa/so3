#!/bin/sh

set -e

ROOTFS_PATH="/persistence/rootfs.fat.virt32"
FILESYSTEM_PATH="/persistence/sdcard.img.virt32"

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
  mount ${devname}p$1 filesystem/fs 
}

umount_filesystem() {
  umount filesystem/fs
  losetup -D
}

build_usr()  {

  mkdir -p usr/build
  
  cd usr/build
  
  cmake --no-warn-unused-cli \
        -Wno-dev \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_TOOLCHAIN_FILE=../arm_toolchain.cmake  \
        .. 
  
  if [ $? -ne '0' ]; then 
    exit 1
  fi
  
  make -j`nproc`
  
  if [ $? -ne '0' ]; then 
    exit 1
  fi

  cd ../..
}

deploy_usr() {
  mount_rootfs
  cp -r usr/out/* rootfs/fs
  cp -r usr/build/deploy/* rootfs/fs
  umount_rootfs
}

deploy_rootfs() {
  mount_rootfs
  mount_filesystem 1
  rm -rf filesystem/fs/*
  cp -rf rootfs/fs/* filesystem/fs/
  
  # Sometimes, syncing between RAM and FS takes some time
  sleep 1
  
  umount_filesystem
  umount_rootfs
}

deploy_uboot() {
    mkimage -f target/virt32_lvperf.its target/virt32_lvperf.itb
    mount_filesystem 1
    cp target/virt32_lvperf.itb filesystem/fs/virt32.itb
    cp u-boot/uEnv.d/uEnv_virt32.txt filesystem/fs/uEnv.txt
    umount_filesystem
}

deploy () {
  deploy_usr
  deploy_rootfs
  deploy_uboot
}

build_usr
deploy

qemu-system-arm \
  -semihosting \
  -smp 4 \
  -serial mon:stdio  \
  -M virt   -cpu cortex-a15 \
  -device virtio-blk-device,drive=hd0 \
  -drive if=none,file=$FILESYSTEM_PATH,id=hd0,format=raw,file.locking=off \
  -m 1024 \
  -kernel u-boot/u-boot \
  -nographic 
