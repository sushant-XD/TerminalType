# fileManagement library
add_library(fileManagement STATIC
    file_source.cpp
)

# Include directories (if you have headers)
target_include_directories(fileManagement PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fileManagement
)

# Link spdlog
target_link_libraries(fileManagement PUBLIC spdlog::spdlog)
