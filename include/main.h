#pragma once
#include "fileManagement/file_source.h"
#include "gameEngine/input_validation.h"
#include "ui/screenManager.h"
#include "utils/ansi_color_codes.h"
#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

void initializeState(State &state, Config config);
void print_usage();
bool configure(int size, char **args, Config &config);
bool configure(Config &config);
void print_config(Config &config);

void handleRunningState(State &state, char tempChar,
                        screenManager &renderManager, inputValidator &validator,
                        terminalCtrl &terminal,
                        std::chrono::steady_clock::time_point &statsUpdateTime);

void handleMenuState(State &state, char tempChar, screenManager &renderManager,
                     terminalCtrl &terminal, fileOps &fileManager,
                     MenuOpts &selectedSetting);

void handleResultsState(State &state, char tempChar,
                        screenManager &renderManager, terminalCtrl &terminal,
                        fileOps &fileManager, ResultOpts &selectedOption);

void handleSettingsState(State &state);
