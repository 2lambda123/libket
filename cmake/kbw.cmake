add_library(
    kbw
    SHARED
    src/bitwise/kbw.cpp
)

target_link_libraries(
    kbw
    PUBLIC
    ket_quantum_code_executor
)

set_target_properties(
    kbw
    PROPERTIES 
    PREFIX ""
    SUFFIX ".kqe"
)
