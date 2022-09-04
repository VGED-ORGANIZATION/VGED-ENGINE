#include <vged.hpp>

#include <utils/live_file.hpp>

#include <string.h>

using namespace VGED::Engine;

int main(int argc, char** argv) {

    if (argc < 2) {
        std::cout << "Usage app [filename]" << std::endl;
        exit(0);
    }

    LiveFile f{ argv[1] };

    while (true) {

        if (f.was_reloaded()) {

            std::vector<char> mem(f.retrieve_size() + 1);
            memcpy(mem.data(), f.retrieve_contents(), mem.size() - 1);
            mem[mem.size() - 1] = '\0'; // add null terminator
            std::cout << "New data: " << (char*)mem.data() << std::endl;
        }
    }

    return 0;
}