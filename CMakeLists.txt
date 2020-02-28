cmake_minimum_required(VERSION 3.16)

project(libket)

add_compile_options(-std=c++17)
add_compile_options(-Wall)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE CSOURCES wrapper/*.cpp)

add_library(ket SHARED ${SOURCES})
set_target_properties(ket PROPERTIES PUBLIC_HEADER "include/ket;include/ket_extra")

add_library(cket SHARED ${SOURCES} ${CSOURCES})
set_target_properties(cket PROPERTIES PUBLIC_HEADER "wrapper/cket.h")

target_link_libraries(ket ${CONAN_LIBS})
target_link_libraries(cket ${CONAN_LIBS})

install(TARGETS ket cket 
        LIBRARY DESTINATION /usr/lib
        PUBLIC_HEADER DESTINATION /usr/include
)
