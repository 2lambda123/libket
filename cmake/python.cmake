include_directories(${PYTHON_INCLUDE_DIR})

add_library(
    pyket
    SHARED
    EXCLUDE_FROM_ALL
    src/python/libket.cpp
)

set_target_properties(
    pyket
    PROPERTIES
    PREFIX "_"   
)

target_link_libraries(
    pyket
    PUBLIC
    libket
    Boost::python
)

