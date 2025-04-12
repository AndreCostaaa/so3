**This repository contains Dockerfiles related to so3.**

# So3 Environnement 

- [Dockerfile.env](./Dockerfile.env)

An ubuntu based image with necessary prerequisites to build and run so3.

# LVGL Performance Test Docker Images

- [Dockerfile.lvperf_32b](./Dockerfile.lvperf_32b)
- [Dockerfile.lvperf_64b](./Dockerfile.lvperf_64b)

Alpine-based images with `so3` pre-compiled using the `virtXX_lvperf_defconfig` configuration.
These images are designed to run [LVGL](https://lvgl.io/) performance tests.

## Getting Started

**Build** the images from the main repository folder (`so3`):
```bash
# 32-bit Version
docker build . -f docker/Dockerfile.lvperf_32b -t so3-lv_perf32b
# 64-bit Version
docker build . -f docker/Dockerfile.lvperf_64b -t so3-lv_perf64b
```

**Run** the images:
```bash
# 32-bit Version
docker run -it --privileged -v /dev:/dev so3-lv_perf32b
# 64-bit Version
docker run -it --privileged -v /dev:/dev so3-lv_perf64b
```

## Technical Details

When running the Docker image, the following operations are performed:
1. Creates a disk image file
2. Builds `usr` in `Release` mode
3. Deploys `so3`, `u-boot` and `usr` to the disk image
4. Launches `so3` with a standard version of `qemu`

With the `virtXX_lvperf_defconfig` configuration, the kernel will run `lvgl_benchmark.elf` instead of `sh.elf` after startup.

## Adding Additional Dependencies

To install extra dependencies without rebuilding the entire image, you can mount a shell script at `/so3/install_dependencies.sh`.
Please note that the image is based on Alpine Linux.

## Persistence

Running the image repeatedly will execute all steps each time. To improve efficiency, you can mount Docker volumes to preserve data between runs:

- `/persistence` - Stores disk image files
- `/so3/usr/build` - Contains build artifacts

Example with mounted volumes:
```bash
docker run -it --privileged \
    -v /dev:/dev \ 
    -v $(pwd)../so3-persistence:/persistence \
    -v $(pwd)../so3-usr-build-64b:/so3/usr/build \
    so3-lv_perf64b
```

> [!NOTE]  
> We strongly recommend using separate `usr/build` folders for 32-bit and 64-bit versions to avoid compatibility issues.

## Customization Options

### LVGL

The `lvgl` source code is located in `/so3/usr/lib/lvgl` and the configuration in `/so3/usr/lib/lv_conf.h`.
You can mount your own version of LVGL as follows:

```bash
docker run -it --privileged \
    -v /dev:/dev \ 
    -v <lvgl_path>:/so3/usr/lib/lvgl \
    -v <lv_conf_path>:/so3/usr/lib/lv_conf.h \
    so3-lv_perf64b
```

### SO3

While not the primary purpose of these images, you can test a patched version of `so3` by mounting it:

```bash
docker run -it --privileged \
    -v /dev:/dev \ 
    -v <patched_so3.bin>:/so3/so3/so3.bin \
    so3-lv_perf64b
```

### U-boot

Similarly, you can test a custom `u-boot` version:

```bash
docker run -it --privileged \
    -v /dev:/dev \ 
    -v <patched_u-boot>:/so3/u-boot/u-boot \
    -v <new_uenv.d>:/so3/u-boot/uEnv.d \
    so3-lv_perf64b
```
