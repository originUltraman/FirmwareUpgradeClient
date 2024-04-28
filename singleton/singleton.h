#ifndef SINGLETON_H
#define SINGLETON_H
#include <memory>
#include <mutex>
#include <memory>
#include <mutex>
#include <iostream>
#include "spdlog/logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/basic_file_sink.h"

template <typename T>
class Singleton
{
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    static T& Instance()
    {
        std::call_once(initInstanceFlag, [&](){
            instancePtr = new T();
        });
        return *instancePtr;
    }

protected:
    Singleton() = default;
    virtual ~Singleton() = default;

private:
    static T* instancePtr;
    static std::once_flag initInstanceFlag;
};

template <typename T>
T* Singleton<T>::instancePtr;

template <typename T>
std::once_flag Singleton<T>::initInstanceFlag;

template<>
spdlog::logger& Singleton<spdlog::logger>::Instance();

#endif // SINGLETON_H

