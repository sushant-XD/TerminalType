# utils library
add_library(utils STATIC
    logging.cpp
)

# Include directories
target_include_directories(utils PUBLIC
    ${CMAKE_SOURCE_DIR}/include/utils
    ${CMAKE_SOURCE_DIR}/include
)

# Link spdlog to utils (since logging.cpp likely uses it)
target_link_libraries(utils PUBLIC spdlog::spdlog)
