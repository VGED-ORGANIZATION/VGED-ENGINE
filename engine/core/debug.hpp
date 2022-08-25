#pragma once

#include <stdexcept>
#include <iostream>

#include "log.hpp"

#ifndef NDEBUG
#define ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << ":" << __LINE__ << "): " << message << std::endl; \
            std::terminate(); \
        } \
    } while (false)

#else
#define ASSERT(condition, message);

#endif

#define THROW(message) std::cerr << \
    "Error in function " << __func__ << " (" << \
    __FILE__ << ":" << __LINE__, "): " << message << std::endl; std::terminate()