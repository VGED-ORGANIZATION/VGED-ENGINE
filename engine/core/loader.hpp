#include "vged.hpp"

namespace VGED {
namespace Engine {

/**
 * @brief Load .so or .dll files, and access functions within them
 *
 */
namespace HotLoader {

enum LoadingModes {
    LOAD_LAZY = 0,
    LOAD_NOW
};

void loadLibrary(std::string path, LoadingModes mode);

} // HotLoader

} // Engine
} // VGED