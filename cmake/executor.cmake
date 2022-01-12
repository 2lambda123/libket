add_library(
    ket_quantum_code_executor 
    STATIC 
    EXCLUDE_FROM_ALL
    src/quantum_code/executor.cpp
    src/quantum_code/executor_run.cpp
    src/quantum_code/executor_exception.cpp
)

target_link_libraries(
    ket_quantum_code_executor
    PUBLIC
    Boost::serialization
    Threads::Threads
)

set_target_properties(
    ket_quantum_code_executor 
    PROPERTIES POSITION_INDEPENDENT_CODE ON
)

target_compile_options(
    ket_quantum_code_executor
    PRIVATE -Wall -Wextra -Wpedantic -Werror
)
