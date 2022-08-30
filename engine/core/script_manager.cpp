#include "script_manager.hpp"

namespace VGED::Engine {

using HotLoader::SharedLibrary;

Result<Script> ScriptManager::get_script(std::string &path) {
	for (auto s : script_functions_) {
		if (s.second.path.compare(path) == 0) {
			Script fn = s.second.func();
			return fn;
		}
	}

	try {
		load_script(path);
		return get_script(path);
	} catch (std::exception e) {
		THROW(e.what());
	}

	return Result<Script>::FAILURE();
}

void ScriptManager::delete_script(std::string &path) {
	ScriptObject &s = script_functions_.find(path)->second;

	if (--s.ref_count <= 0) {
		unload_script(path);
	}
}

void ScriptManager::load_script(const std::string &path) {

	SharedLibrary lib{ path, HotLoader::LOAD_NOW };
	auto func = lib.get_function<ScriptCreateFunctionPointer>(
					   "engine_get_script_object")
					.expect("Failed to find function in script"); // TODO handle

	ScriptObject script{ func, lib, path };

	script_functions_.insert(
		std::pair<std::string, ScriptObject>(path, script));
}

void ScriptManager::unload_script(const std::string &path) {
	script_functions_.erase(path); // should auto call destructors? OOP smh
}

void ScriptNameRegistry::register_name(const std::string &path,
									   const std::string &name) noexcept {

	registry.insert(name, path);
}

void ScriptNameRegistry::unregister_name(const std::string &name) noexcept {
	registry.erase(name);
}

Result<std::string> ScriptNameRegistry::get_path(const std::string &name) {

	if (registry.contains(name))
		return Result<std::string>::SUCCESS(registry.at(name));
	else
		return Result<std::string>::FAILURE();
}

} // VGED::Engine