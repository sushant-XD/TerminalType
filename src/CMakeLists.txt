add_executable(termtype main.cpp file_source.cpp terminal_ctrl.cpp input_validation.cpp design.cpp)
target_include_directories(termtype PRIVATE "${CMAKE_SOURCE_DIR}/include")
