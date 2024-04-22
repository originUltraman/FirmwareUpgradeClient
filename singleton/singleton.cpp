#include "singleton.h"
template<>
spdlog::logger& Singleton<spdlog::logger>::Instance(){
    std::call_once(initInstanceFlag, [&]() {
        auto max_size = 1048576 * 5;
        auto max_files = 3;
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::trace);
        console_sink->set_pattern("%v");

        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/log.txt", max_size, max_files);
        file_sink->set_level(spdlog::level::trace);
        file_sink->set_pattern("[%Y-%m-%d %H:%M:%S][process:%P][thread: %t][%s:%#][%l][%n], %v");
        instancePtr.reset(new spdlog::logger("logger", { console_sink, file_sink }));

        });
    return *instancePtr;
}
