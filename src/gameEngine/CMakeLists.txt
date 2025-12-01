# gameEngine library
add_library(gameEngine STATIC
    input_validation.cpp
    terminal_ctrl.cpp
)

# Include directories
target_include_directories(gameEngine PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gameEngine
)

# Link spdlog
target_link_libraries(gameEngine PUBLIC spdlog::spdlog)

# Link other dependencies if needed
# target_link_libraries(gameEngine PRIVATE utils)
