
add_executable(sh.elf sh.c)
add_executable(ls.elf ls.c)
add_executable(more.elf more.c)
add_executable(time.elf time.c)
add_executable(ping.elf ping.c)
add_executable(mydev_test.elf mydev_test.c)
add_executable(lvgl_perf.elf lvgl_perf.c)

add_subdirectory(slv)

target_link_libraries(sh.elf c)
target_link_libraries(ls.elf c)
target_link_libraries(more.elf c)
target_link_libraries(time.elf c)
target_link_libraries(ping.elf c)
target_link_libraries(mydev_test.elf c)
target_link_libraries(lvgl_perf.elf c slv lvgl lvgl_demos)

if (MICROPYTHON AND (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64"))
	message("== Building uPython")
	add_subdirectory(micropython)
endif()
