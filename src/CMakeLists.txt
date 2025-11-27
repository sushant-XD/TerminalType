add_executable(termtype
main.cpp
file_source.cpp
terminal_ctrl.cpp 
input_validation.cpp 
design.cpp 
logging.cpp
uiWidgets.cpp
canvas.cpp
menuScreen.cpp
gameScreen.cpp
screenManager.cpp
)

target_include_directories(termtype PRIVATE "${CMAKE_SOURCE_DIR}/include")
if(NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif()

target_link_libraries(termtype PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
include(GNUInstallDirs)
install(TARGETS termtype
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
