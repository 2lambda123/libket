add_library(
    kbw
    SHARED
    src/bitwise/kbw.cpp
)

target_link_libraries(
    kbw
    PUBLIC
    ket_quantum_code_executor
    Boost::filesystem
)

set_target_properties(
    kbw
    PROPERTIES 
    PREFIX ""
)

add_library(
    kbw_plugins
    SHARED
    src/bitwise/pown.cpp
)

set_target_properties(
    kbw_plugins
    PROPERTIES 
    PREFIX ""
)
