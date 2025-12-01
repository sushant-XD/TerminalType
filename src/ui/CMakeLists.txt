# ui library
add_library(ui STATIC
    screenManager.cpp
    screens/canvas.cpp
    screens/gameScreen.cpp
    screens/menuScreen.cpp
    screens/resultScreen.cpp
    widgets/uiWidgets.cpp
)

# Include directories
target_include_directories(ui PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ui
)

# Link dependencies
target_link_libraries(ui PUBLIC
    spdlog::spdlog
    gameEngine
    utils
)
