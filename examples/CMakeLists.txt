include_directories(../include)

add_executable(random random.cpp)
target_link_libraries(random ket)
set_target_properties(random PROPERTIES CXX_STANDARD 17)

add_executable(teleport teleport.cpp)
target_link_libraries(teleport ket)
set_target_properties(teleport PROPERTIES CXX_STANDARD 17)

add_executable(shor15 shor15.cpp)
target_link_libraries(shor15 ket)
set_target_properties(shor15 PROPERTIES CXX_STANDARD 17)
