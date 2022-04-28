cmake_minimum_required(VERSION 3.15)
project(libket)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR})

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                EXPECTED_HASH SHA256=396e16d0f5eabdc6a14afddbcfff62a54a7ee75c6da23f32f7a31bc85db23484
                TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

if(DEFINED CONAN_BOOST_ALL)
  conan_cmake_run(
    REQUIRES 
    boost/1.76.0
    BASIC_SETUP 
    CMAKE_TARGETS
    BUILD missing
  )
else()
  conan_cmake_run(
    REQUIRES 
    boost/1.76.0
    BASIC_SETUP 
    CMAKE_TARGETS
    BUILD missing
    OPTIONS
    boost:without_atomic=True
    boost:without_chrono=True
    boost:without_container=True
    boost:without_context=True
    boost:without_contract=True
    boost:without_coroutine=True
    boost:without_date_time=True
    boost:without_exception=True
    boost:without_fiber=True
    boost:without_filesystem=False
    boost:without_graph=True
    boost:without_graph_parallel=True
    boost:without_iostreams=True
    boost:without_json=True
    boost:without_locale=True
    boost:without_log=True
    boost:without_math=True
    boost:without_mpi=True
    boost:without_nowide=True
    boost:without_program_options=True
    boost:without_python=True
    boost:without_random=True
    boost:without_regex=True
    boost:without_serialization=False
    boost:without_stacktrace=True
    boost:without_system=False
    boost:without_test=True
    boost:without_thread=True
    boost:without_timer=True
    boost:without_type_erasure=True
    boost:without_wave=True
  )
endif()

include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(Boost_INSTALL_DIR ${CONAN_BOOST_ROOT})
set(Boost_USE_STATIC_LIBS ON)

find_package(Boost
  1.76.0 EXACT
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
