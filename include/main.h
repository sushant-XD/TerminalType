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
void initializeState(state_t &state);
void print_usage();
bool configure(int size, char **args, config_s &config);
bool configure(config_s &config);
void print_config(config_s &config);

void handleRunningState(state_t &state, char tempChar,
                        screenManager &renderManager, inputValidator &validator,
                        terminalCtrl &terminal,
                        std::chrono::steady_clock::time_point &statsUpdateTime);

void handleMenuState(state_t &state, char tempChar,
                     screenManager &renderManager, terminalCtrl &terminal,
                     selectedMenuOption &selectedSetting);

void handleResultsState(state_t &state, screenManager &renderManager,
                        terminalCtrl &terminal);

void handleSettingsState(state_t &state);
