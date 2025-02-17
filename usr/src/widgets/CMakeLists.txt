
include_directories(.)

#add_compile_options("-DLV_LVGL_H_INCLUDE_SIMPLE")

add_compile_options("-DLV_USE_DEMO_WIDGETS") 
add_executable(demofb.elf demofb.c)

target_link_libraries(demofb.elf c lvgl lvgl_demos)
