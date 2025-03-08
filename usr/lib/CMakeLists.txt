# Hacky fix to make sure we don't compile external libs with -Werror flag
set(ORIGINAL_C_FLAGS "${CMAKE_C_FLAGS}")
string(REPLACE "-Werror" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

add_subdirectory(libc)
add_subdirectory(lvgl)

set(CMAKE_C_FLAGS "${ORIGINAL_C_FLAGS}")

# Non external libs 

add_subdirectory(slv)
