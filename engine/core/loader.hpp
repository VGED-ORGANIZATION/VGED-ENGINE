#include "core/result.hpp"
#include <vged.hpp>

#include <stdexcept>

namespace VGED {
    namespace Engine {

        inline namespace Core {

            /**
             * @brief Load .so or .dll files, and access functions within them
             *
             */
            namespace HotLoader {

                enum LoadingModes { LOAD_LAZY = 0,
                                    LOAD_NOW };

                class SharedLibrary {

                public:
                    SharedLibrary(std::string path, LoadingModes mode = LOAD_LAZY);
                    ~SharedLibrary();

                    /**
                     * @brief Get the function object
                     *
                     * @tparam T the type of the function
                     * @param name the name of the function
                     * @return T a pointer to the function, or NULL
                     */
                    template <typename T>
                    Result<T> get_function(std::string name);

                    const void* get_lib(void) { return dlptr_; }

                private:
                    void* dlptr_;
                };

                /**
                 * @brief Store and execute loaded code
                 *
                 */
                class Executable {

                public:
                    /**
                     * @brief Construct a new Executable object
                     *
                     * @param path the path to the shared object
                     * @param entryPoint the main function name
                     * @param mode the method of loading the file
                     */
                    Executable(std::string path, std::string entry_point = "main",
                               LoadingModes mode = LOAD_LAZY);

                    ~Executable(void);

                    // TODO deep copy
                    Executable operator=(Executable&) = delete;
                    Executable(Executable&) = delete;

                    int run(std::vector<std::string> args);

                    /**
                     * @brief Change the libary loaded in the executable
                     *
                     * @param path the path to the new shared object
                     * @return int
                     */
                    int change_library(std::string path, LoadingModes load_mode = LOAD_LAZY);

                    /**
                     * @brief Get the libary memory
                     *
                     * @return void*
                     */
                    inline const SharedLibrary* get_libary(void) { return lib_; }

                private:
                    SharedLibrary* lib_ = nullptr;
                    std::string entry_point_;
                    LoadingModes load_mode_;
                };

            } // HotLoader
        } // Core
    } // Engine
} // VGED
