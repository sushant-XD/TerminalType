// logging.cpp
#include "logging.h"
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
void init_logger(const std::string &name, const std::string &filePath,
                 spdlog::level::level_enum level) {
  if (spdlog::get(name)) {
    return;
  }

  // 2. Initialize spdlog's thread pool
  // This must be done BEFORE creating the async logger
  spdlog::init_thread_pool(8192, 1); // 8192 queue size, 1 background thread

  // 3. Create an ASYNC file sink
  auto file_sink =
      std::make_shared<spdlog::sinks::basic_file_sink_mt>(filePath, true);
  file_sink->set_level(level);

  // 4. Create an ASYNC logger
  auto new_logger = std::make_shared<spdlog::async_logger>(
      name, spdlog::sinks_init_list({file_sink}), spdlog::thread_pool(),
      spdlog::async_overflow_policy::block);

  // Set the logger's global level
  new_logger->set_level(spdlog::level::debug);

  // 5. You can KEEP flush_on.
  // With async, this just tells the background thread to flush,
  // it does NOT block your main application thread.
  new_logger->flush_on(spdlog::level::debug);

  spdlog::register_logger(new_logger);
  spdlog::set_default_logger(new_logger);
}
