cmake_minimum_required(VERSION 3.20)

project(TerminalType VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Debug)

if(NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif()

add_subdirectory(src/fileManagement)
add_subdirectory(src/gameEngine)
add_subdirectory(src/ui)
add_subdirectory(src/utils)

add_executable(${PROJECT_NAME} main.cpp)


# link all libraries to main executable
target_link_libraries(${PROJECT_NAME} PRIVATE
    fileManagement
    gameEngine
    ui
    utils
    spdlog::spdlog
)
