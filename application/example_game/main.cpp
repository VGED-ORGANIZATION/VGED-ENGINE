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
            std::cout << "change reported, size: " << f.retrieve_size() << '\n';
            struct stat s;
            stat(argv[1], &s);
            std::cout << "stat says size: " << s.st_size << std::endl;
        }
    }

    return 0;
}