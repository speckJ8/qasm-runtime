add_library(Gate gate.cc)
add_library(Runtime runtime.cc)
add_library(State state.cc)

add_subdirectory(math)

target_link_libraries(Gate PUBLIC Math)
target_link_libraries(State PUBLIC Math)
target_link_libraries(Runtime PUBLIC Gate State)

target_include_directories(Runtime PUBLIC "${CMAKE_SOURCE_DIR}")
