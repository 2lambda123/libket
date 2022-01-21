cmake_minimum_required(VERSION 3.15)
project(libket)

include_directories(${CMAKE_SOURCE_DIR})

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                EXPECTED_HASH SHA256=396e16d0f5eabdc6a14afddbcfff62a54a7ee75c6da23f32f7a31bc85db23484
                TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_run(
  REQUIRES 
  boost/1.77.0
  BASIC_SETUP 
  CMAKE_TARGETS
  BUILD missing
)

include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(Boost_INSTALL_DIR ${CONAN_BOOST_ROOT})
set(Boost_USE_STATIC_LIBS ON)

find_package(Boost
  1.77.0 EXACT
  REQUIRED
  COMPONENTS
  serialization
  filesystem
)     

find_package(Threads REQUIRED)

include(cmake/libket.cmake)
include(cmake/executor.cmake)
include(cmake/kbw.cmake)
include(cmake/examples.cmake)
