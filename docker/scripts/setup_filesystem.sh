#!/bin/sh

set -e

target_path="/persistence/sdcard.img.$PLATFORM"

if [ -f "$target_path" ]; then
    echo "Found existing filesystem image in $target_path."
else 
  mkdir -p /persistence
  echo "No existing rootfs image found in $target_path. Creating new one..."
  dd_size=256M
  dd if=/dev/zero of=$target_path bs="$dd_size" count=1
  devname=$(losetup --partscan --find --show $target_path)
  
  (echo o; echo n; echo p; echo; echo; echo; echo t; echo c; echo w) | fdisk $devname
  
  mkfs.fat -F32 -v "$devname"p1
  losetup -D
fi
