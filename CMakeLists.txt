cmake_minimum_required(VERSION 3.15)

project(libket)

if(NOT CMAKE_BUILD_TYPE) 
    set(CMAKE_BUILD_TYPE Debug)
endif()

add_compile_options(-Wall -Wextra -pedantic)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
                  "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

if (NOT TARGET CONAN_PKG::boost)
    conan_cmake_run(REQUIRES boost/1.71.0
                    BASIC_SETUP CMAKE_TARGETS
                    BUILD missing)
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

find_package(Threads REQUIRED)
find_package(GMP REQUIRED)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_library(ket SHARED ${SOURCES})
set_target_properties(ket PROPERTIES 
                      PUBLIC_HEADER "include/ket"
                      CXX_STANDARD 17)

target_link_libraries(ket CONAN_PKG::boost Threads::Threads gmp ssh)

install(TARGETS ket) 
    
add_subdirectory(examples)