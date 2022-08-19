#ifndef CORE_DEBUG_H
#define CORE_DEBUG_H

#include <stdexcept>
#include <iostream>

#ifndef NDEBUG 
#define ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " (" << __LINE__ << "): " << message << std::endl; \
            std::terminate(); \
        } \
    } while (false)

#else
#define ASSERT(condition, message);

#endif

#define THROW(message) std::cerr << "Error in file " << __FILE__ << "(" << __LINE__ << "): " << message << std::endl; std::terminate()

#endif