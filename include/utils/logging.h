#pragma once

#include <spdlog/spdlog.h>
#include <string>

/**
 * @brief Initializes the global default logger.
 * @param name The name to register the logger with (e.g., "my_app").
 * @param filePath The path to the log file (e.g., "logs/app.log").
 * @param level The minimum level to log to the file.
 */
void init_logger(const std::string &name, const std::string &filePath,
                 spdlog::level::level_enum level);
