cmake_minimum_required(VERSION 3.15)

project(libket)

add_compile_options(-Wall -Wextra -pedantic -Werror)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
                    "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_run(CONFIGURATION_TYPES "Release;Debug")

if (NOT TARGET CONAN_PKG::boost)
    conan_cmake_run(REQUIRES boost/1.71.0
                    BASIC_SETUP CMAKE_TARGETS
                    BUILD missing)
endif()

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE CSOURCES wrapper/*.cpp)

add_library(ket SHARED ${SOURCES})
set_target_properties(ket PROPERTIES 
                      PUBLIC_HEADER "include/ket"
                      CXX_STANDARD 17)
target_link_libraries(ket CONAN_PKG::boost)

add_library(cket SHARED ${SOURCES} ${CSOURCES})
set_target_properties(cket PROPERTIES
                      PUBLIC_HEADER "wrapper/cket.h"
                      CXX_STANDARD 17)
target_link_libraries(cket CONAN_PKG::boost)

install(TARGETS ket cket 
        LIBRARY DESTINATION /usr/lib
        PUBLIC_HEADER DESTINATION /usr/include)

install(DIRECTORY include/ket_bits DESTINATION /usr/include)
