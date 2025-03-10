#!/bin/sh

set -e

PLATFORM=$1

target_path="/persistence/rootfs.fat.$PLATFORM"

if [ -f "$target_path" ]; then
    echo "Found existing rootfs image in $target_path."
else 
  mkdir -p /persistence
  echo "No existing rootfs image found in $target_path. Creating new one..."
  start_sector=2048
  partition_size=16M # This image will be copied into the .itb and written to SD card image so it must be small enough
  partition_type=c
  tmp_dir=$(mktemp -d -t so3-rootfs-XXXXXXXX)
  partition="${tmp_dir}/partition.img"
  
  # Create image first
  dd if=/dev/zero of="${target_path}" count=${start_sector} status=none
  
  # Append the formatted partition
  dd if=/dev/zero of="${partition}" bs=${partition_size} count=1 status=none
  mkfs.vfat "${partition}"
  dd if="${partition}" status=none >> "${target_path}"
    
  # Set the partition table
  sfdisk "${target_path}" <<EOF
${start_sector}, ${partition_size}, ${partition_type}
EOF
    
  # Delete temporary directory
  rm -r "${tmp_dir}"
fi

# rootfs/rootfs.fat is used inside the .its file
ln -sf $target_path rootfs/rootfs.fat
