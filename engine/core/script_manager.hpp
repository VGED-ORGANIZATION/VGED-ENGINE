/**
 * @file script_manager.hpp
 * @author Kael Johnston (wkaelj@gmail.com)
 * @brief Manage scripts for the ecs, to ensure that the same script does not
 * get loaded twice. All scripts must can include an init, update, destroy
 * method that will be called on initialization, per frame, and on destruction
 * respectivly.
 *
 * @version 0.1
 * @date 2022-08-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <list>
#include <map>
#include <vged.hpp>

#include "core/script_header.hpp"
#include "core/types.hpp"
#include "loader.hpp"

namespace VGED {
    namespace Engine {
        inline namespace Core {

            /**
             * @brief Manage script modules loaded by the ecs
             * it's not just a global variable i promise.
             *
             */
            class ScriptManager {

            public:
                ScriptManager() = delete;
                ~ScriptManager() = delete;

                Result<Script> get_script(std::string& path);
                void delete_script(std::string& path);

            private:
                struct ScriptObject;

                // store the function to create scripts for each object
                std::map<std::string, ScriptObject> script_functions_;

                void load_script(const std::string& path);
                void unload_script(const std::string& path);

                /**
                 * @brief Store a script object and its reference count
                 *
                 */
                struct ScriptObject {

                    inline ScriptObject(ScriptCreateFunctionPointer f,
                                        HotLoader::SharedLibrary l, std::string filename)
                        : func(f), lib{ l }, path{ filename } {}

                    const ScriptCreateFunctionPointer func = nullptr;
                    size_t ref_count = 0;
                    const HotLoader::SharedLibrary lib;
                    const std::string path;
                };
            };

            class ScriptNameRegistry {

            public:
                ScriptNameRegistry() = delete;
                ~ScriptNameRegistry() = delete;

                void register_name(const std::string& path,
                                   const std::string& name) noexcept;
                void unregister_name(const std::string& name) noexcept;

                Result<std::string> get_path(const std::string& name);

            private:
                std::map<std::string, std::string> registry; // name, path
            };

        } // Core
    } // Engine
} // VGED
