cmake_minimum_required(VERSION 3.20)
project(
  TerminalType
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Debug)

# Set local install by default
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX
      "${CMAKE_BINARY_DIR}/install"
      CACHE PATH "Default local install directory" FORCE)
endif()

if(NOT TARGET spdlog)
  find_package(spdlog REQUIRED)
endif()

add_subdirectory(src/fileManagement)
add_subdirectory(src/gameEngine)
add_subdirectory(src/ui)
add_subdirectory(src/utils)

add_executable(${PROJECT_NAME} main.cpp)

# Pass install paths to your code
target_compile_definitions(
  ${PROJECT_NAME} PRIVATE INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
                          PROJECT_NAME="${PROJECT_NAME}")

target_link_libraries(${PROJECT_NAME} PRIVATE fileManagement gameEngine ui
                                              utils spdlog::spdlog)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES EasyLevel.txt MediumLevel.txt HardLevel.txt
        DESTINATION share/${PROJECT_NAME})
