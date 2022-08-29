#include "loader.hpp"
#include "core/types.hpp"

#include <dlfcn.h>

namespace VGED {
namespace Engine {

namespace HotLoader {

inline int convert_loading_mode(LoadingModes mode)
{
    switch (mode) {
    case LOAD_LAZY: return RTLD_LAZY;
    case LOAD_NOW: return RTLD_NOW;
    default: return RTLD_LAZY;
    }
}

SharedLibrary::SharedLibrary(std::string path, LoadingModes mode)
{

    dlptr_ = dlopen(path.c_str(), convert_loading_mode(mode));
    if (dlptr_ == NULL)
    {
        std::string err = "Failed to load shared object: ";
        err = err.append(path);
        throw std::runtime_error(err); // why no printf formatting :(
    }
}

SharedLibrary::~SharedLibrary()
{
    dlclose(dlptr_);
}

template <typename T>
inline Result<T> SharedLibrary::get_function(std::string name)
{
    auto f = (T) dlsym(dlptr_, name.c_str());
    if (f) return Result<T>::SUCCESS(f);
    else return Result<T>::FAILURE();
}

Executable::Executable(
    std::string path,
    std::string entryPoint,
    LoadingModes mode) : entryPoint_(entryPoint)
{
    lib_ = new SharedLibrary(path, mode);
    return;
}

Executable::~Executable()
{
    delete lib_;
}

int Executable::run(std::vector<std::string> args)
{
    int (*main)(int, char**) = lib_->get_function<int(*)(int, char**)>(entryPoint_);

    // convert args
    char *cargs[args.size()];
    for (size_t i = 0; i < args.size(); ++i) {
        cargs[i] = (char*) args[i].c_str(); // FIXME idk probably shouldn't cast
    }
    return main(args.size(), cargs);
}

int Executable::change_library(std::string path, LoadingModes loadMode)
{
    delete lib_;
    lib_ = new SharedLibrary(path, loadMode);

    if (lib_ == NULL) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

} // HotLoader

} // Engine
} // VGED