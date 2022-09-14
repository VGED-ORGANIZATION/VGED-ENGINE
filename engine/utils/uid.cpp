#include "uid.hpp"

namespace VGED::Engine::Utils::UID {

uid_t generate(void) {
    static u64 current_id = 0;
    return current_id++ + time(NULL);
}

} // VGED::Engine::Utils::UID