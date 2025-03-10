#!/bin/sh


create_filesystem_image() {
  dd_size=256M
  dd if=/dev/zero of=filesystem/sdcard.img.virt32 bs="$dd_size" count=1
  devname=$(losetup --partscan --find --show filesystem/sdcard.img.virt32)
  
  devname=${devname#"/dev/"}
  
  (echo o; echo n; echo p; echo; echo; echo; echo t; echo c; echo w) | fdisk /dev/"$devname"
  
  sleep 2s
  
  if [[ "$devname" = *[0-9] ]]; then
      export devname="${devname}p"
  fi
  
  mkfs.fat -F32 -v /dev/"$devname"p1
  mkfs.ext4 /dev/"$devname"p2
  losetup -D
}

create_ramfs() {
  start_sector=2048
  partition_size=16M # This image will be copied into the .itb and written to SD card image so it must be small enough
  partition_type=c
  tmp_dir=$(mktemp -d -t so3-rootfs-XXXXXXXX)
  partition="${tmp_dir}/partition.img"
  
  # Create image first
  image_name="rootfs/rootfs.fat"
  dd if=/dev/zero of="${image_name}" count=${start_sector} status=none
  
  # Append the formatted partition
  dd if=/dev/zero of="${partition}" bs=${partition_size} count=1 status=none
  mkfs.vfat "${partition}"
  dd if="${partition}" status=none >> "${image_name}"
  
  # Set the partition table
sfdisk "${image_name}" <<EOF
${start_sector}, ${partition_size}, ${partition_type}
EOF
  
  # Delete temporary directory
  rm -r "${tmp_dir}"
}

mount_rootfs() {
  mkdir -p rootfs/fs
  device=$(losetup --partscan --find --show rootfs/rootfs.fat)
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

  devname=$(losetup --partscan --find --show filesystem/sdcard.img.virt32)
  mount ${devname}p$1 filesystem/fs 
}

umount_filesystem() {
  umount filesystem/fs
  losetup -D
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

cd usr; ./build.sh -r; cd ..

create_ramfs
create_filesystem_image
deploy


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
