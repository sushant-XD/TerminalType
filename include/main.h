#pragma once

#include "ansi_color_codes.h"
#include "design.h"
#include "file_source.h"
#include "input_validation.h"
#include "terminal_ctrl.h"
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
