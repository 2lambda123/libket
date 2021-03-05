include_directories(../include)

add_executable(random random.cpp)
target_link_libraries(random ket)
set_target_properties(random PROPERTIES CXX_STANDARD 17)

add_executable(teleport teleport.cpp)
target_link_libraries(teleport ket)
set_target_properties(teleport PROPERTIES CXX_STANDARD 17)

add_executable(dump_state dump_state.cpp)
target_link_libraries(dump_state ket)
set_target_properties(dump_state PROPERTIES CXX_STANDARD 17)