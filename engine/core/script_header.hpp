/**
 * @file script_header.hpp
 * @author Kael name (you@domain.com)
 * @brief This file must be included by all scripts
 * @version 0.1
 * @date 2022-08-24
 *
 * @copyright Copyright (c) 2022
 *
 */

/**
 * @brief this header file must be included by all scripts
 *
 */

#pragma once

#include <cstdlib>

namespace VGED {
namespace Engine {
inline namespace Core {

class Script {

public:
	Script() = delete;

	virtual void init(void);

	virtual void update(std::size_t delta);

	virtual void destroy(void);
};

/**
 * @brief Used to store script create functions
 *
 */
using ScriptCreateFunctionPointer = Script (*)(void);

/**
 * @brief Used by the engine to get the user written script inside the function.
 * this function must be implemented to return an instance of the users class
 *
 * @return Script
 */
extern Script
engine_get_script_object(void); // FIXME maybe extern "C"?? but warns

} // Core
} // Engine
} // VGED