add_library(
    libket
    STATIC
    src/libket/process.cpp
    src/libket/process_run.cpp
    src/libket/process_exception.cpp
    src/libket/block.cpp
    src/libket/dump.cpp
    src/libket/future.cpp
    src/libket/future_op.cpp
    src/libket/label.cpp
    src/libket/qubit.cpp
)

set_target_properties(
    libket 
    PROPERTIES 
    POSITION_INDEPENDENT_CODE ON
    PREFIX ""
)

target_link_libraries(
    libket
    PUBLIC
    Boost::serialization
    Boost::filesystem
    Threads::Threads
    ${CMAKE_DL_LIBS}
)

target_compile_options(
    libket
    PRIVATE -Wall -Wextra -Wpedantic -Werror -Wno-abi
)