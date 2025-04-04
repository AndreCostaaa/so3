#
# CMake toolchain information for SO3 build
# Inspired from buildroot generated toolchain.cmake
#

set(CMAKE_SYSTEM_NAME SO3_usr)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_C_FLAGS_DEBUG "" CACHE STRING "Debug CFLAGS")
set(CMAKE_C_FLAGS_RELEASE " -DNDEBUG" CACHE STRING "Release CFLAGS")

set(CMAKE_BUILD_TYPE Release CACHE STRING "SO3 user applications build system")

set(CMAKE_EXE_LINKER_FLAGS "" CACHE STRING "SO3 usr LDFLAGS for executables")

set(CMAKE_INSTALL_SO_NO_EXE 0)

# This toolchain file can be used both inside and outside Buildroot.
set(CMAKE_C_COMPILER "aarch64-none-elf-gcc")
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_CXX_COMPILER "aarch64-none-elf-g++")
set(CMAKE_C_LINK_EXECUTABLE "aarch64-none-elf-ld <OBJECTS> -o <TARGET>  <LINK_LIBRARIES> <LINK_FLAGS> <LINK_LIBRARIES>")
set(CMAKE_ASM_COMPILER "aarch64-none-elf-gcc")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c99 -D_GNU_SOURCE -nostdlib -D__ARM64__ -ffreestanding -fno-common")
 
set(CMAKE_ASM_FLAGS "-D__ARM64__ -D__ASSEMBLY__")

set(CMAKE_LINKER "aarch64-none-elf-ld")
