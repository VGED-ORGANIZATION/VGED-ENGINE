#pragma once

#include <vged.hpp>

namespace VGED::Engine::Utils {
namespace UID {

typedef std::uint64_t uid_t;

/**
 * @brief Create a new uid. UID are only unique for this run of the program
 *
 * @return uid_t
 */
uid_t generate(void);

} // UID
} // VGED::Engine::Utils

