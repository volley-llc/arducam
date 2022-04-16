cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_BUILD_TYPE Debug)

project(ArduCam VERSION 0.1.0)
set(CMAKE_C_STANDARD 99)



include(CTest)
enable_testing()

set(SOURCE_FILES acam_control.c acam_control.h)
add_library(ArduCam STATIC ${SOURCE_FILES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_compile_options(main PUBLIC 
-Wall 
-Wextra 
-Warray-bounds 
-Wcast-qual 
-Wclobbered 
-Wduplicated-branches
-Wduplicated-cond
-Wlogical-op 
-Wdouble-promotion
-Wformat=2
-Wmaybe-uninitialized 
-Wmissing-field-initializers 
-Wno-free-nonheap-object
-Wnull-dereference
-Wpointer-arith 
-Wstrict-aliasing 
-Wstrict-overflow 
-Wunreachable-code 
-Wunused-but-set-variable 
-Wwrite-strings
)
