#pragma once

#include <stdexcept>
#include <iostream>

#include "log.hpp"

#ifndef NDEBUG
#define ASSERT_MSG(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Asserton (" << #condition << \
            ") failed in " << __func__ << " (" << \
            __FILE__ << ":" << __LINE__, "): " << \
            message << std::endl; std::terminate(); \
        } \
    } while (false)

#else
#define ASSERT_MSG(condition, message) ((void)0)
#endif

#ifdef NDEBUG
#define ASSERT(condition) \
    do { \
        if (!(condition)) { \
            std::cerr << "Asserton (" << #condition << \
            ") failed in " << __func__ << " (" << \
            __FILE__ << ":" << __LINE__, ")" << std::endl; \
            std::terminate(); \
        } \
    } while(0)
#else
#define ASSERT(condition) ((void)0)
#endif

#define THROW(message) \
    do { \
        std::cerr << "Error in function " << __func__ << " (" << \
        __FILE__ << ":" << __LINE__ << "): " << message << std::endl; \
        std::terminate(); \
    } while(0)