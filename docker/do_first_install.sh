#!/bin/sh

# 1. Create FileSystem Image
dd if=/dev/zero of=filesystem/sdcard.img.virt32 bs=256M count=1
DEVNAME=$(losetup --partscan --find --show sdcard.img.virt32)
(echo o; echo n; echo p; echo; echo; echo; echo t; echo c; echo w) | fdisk $DEVNAME;

mkfs.fat -F32 -v ${DEVNAME}1
mkfs.ext4 ${DEVNAME}2


# 2. Create Rootfs

mkdir /so3/tmp
START_SECTOR=2048
PARTITION_SIZE=16M
PARTITION_TYPE=c
PARTITION="/so3/tmp/partition.img"

# Create image first
IMAGE_NAME="rootfs/rootfs.fat"
dd if=/dev/zero of="${IMAGE_NAME}" count=${START_SECTOR} status=none

# Append the formatted partition
dd if=/dev/zero of="${PARTITION}" bs=${PARTITION_SIZE} count=1 status=none
mkfs.vfat "${PARTITION}" > /dev/null
dd if="${PARTITION}" status=none >> "${IMAGE_NAME}"

# Set the partition table
sfdisk "${IMAGE_NAME}" <<EOF
${START_SECTOR}, ${PARTITION_SIZE}, ${PARTITION_TYPE}
EOF

rm -rf /so3/tmp

mkdir filesystem/fs

# 2. Make u-boot
mkimage -f target/virt32_lvperf.its target/virt32_lvperf.itb;

mount $(losetup --partscan --find --show filesystem/sdcard.img.virt32)p1 filesystem/fs

cp target/virt32_lvperf.itb filesystem/fs
cp ../u-boot/uEnv.d/uEnv_virt32.txt fs/uEnv.txt

umount filesystem/fs
losetup -D

echo "OK" > installation_done
