add_executable(
    teleport 
    EXCLUDE_FROM_ALL
    examples/teleport.cpp
)

target_link_libraries(
    teleport
    PUBLIC
    libket
)

add_executable(
    bell 
    EXCLUDE_FROM_ALL
    examples/bell.cpp
)

target_link_libraries(
    bell
    PUBLIC
    libket
)