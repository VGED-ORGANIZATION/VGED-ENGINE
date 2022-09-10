#include "live_file.hpp"

#include <ctime>
#include <cstdio>
#include <filesystem>
#include <map>

namespace VGED {
namespace Engine {
inline namespace Core {

//
// LiveFileManager Decleration
//

namespace LiveFileManager {

struct LoadedFile;

std::map<uid_t, std::unique_ptr<LoadedFile>> m_files;
std::map<std::string, uid_t> m_path_registry;

volatile bool m_stop_thread = false; // used to stop the thread
std::thread* m_testthread;

void* thread_main();

void init_manager(void) __attribute__((constructor));
void terminate_manager(void) __attribute((destructor));

Result<const void*> access_file(const uid_t id);
Result<std::size_t> get_file_size(const uid_t id);

// iterate through files and check if any need to be reloaded
void check_for_reloads(void);

u64 get_file_generation(const uid_t id);

// check out and return files, so the manager knows when they can be
// released
Result<uid_t> checkout_file(const std::string& path);
Result<uid_t> checkout_file(const uid_t id);

void return_file(const uid_t id);
void return_file(const std::string& path);

/**
 * @brief Basically a class
 *
 */
class LoadedFile {

public:
    LoadedFile(const std::string& filepath) : path{ filepath } {

        load_file(path);

        // initialize values
        id = Utils::UID::generate();
        reference_count = 0;
        last_loaded_time = std::filesystem::file_time_type::clock::now();
        file_generation = 0;
    }

    LoadedFile(const std::string& filepath, uid_t _id) : path{ filepath } {

        load_file(path);

        // initialize values
        id = _id;
        reference_count = 0;
        last_loaded_time = std::filesystem::file_time_type::clock::now();
        file_generation = 0;
    }

    ~LoadedFile() { free(fileptr); }

    bool operator==(const LoadedFile& lf) { return id == lf.id; }
    void* operator*(void) { return fileptr; }

    // getters

    // get the number of times the file has been reloaded
    u32 get_generation(void) { return file_generation; };
    // get the unique id of the file
    uid_t get_id(void) { return id; }
    // get the size of the file, in bytes
    std::size_t get_size(void) { return filesize; }
    // get a void pointer to the contents of the file
    const void* get_contents(void) { return fileptr; }
    // get the path to the file
    std::string& get_path(void) { return path; }

    /**
     * @brief reload the file
     *
     * @return true
     * @return false
     */
    bool reload_file(void) __THROW {

        file_generation++;
        last_loaded_time = std::filesystem::file_time_type::clock::now();
        free(fileptr);
        return load_file(path);
    }
    /**
     * @brief Check if the file needs to be reloaded
     *
     * @return true
     * @return false
     */
    bool needs_reload(void) {
        auto modified = std::filesystem::last_write_time(path);
        if (modified.time_since_epoch() > last_loaded_time.time_since_epoch())
            return true;
        else
            return false;
    }

    uid_t checkout_file(void) {
        reference_count++;
        return id;
    }

    void return_file(void) {
        reference_count--;
        if (reference_count == 0) {
            // FIXME possibly unload
        }
    }

    bool in_use(void) { return reference_count > 0; }

private:
    // used in accessing the file
    u32 reference_count;
    uid_t id;

    // used to interface with the filesystem
    std::string path;
    void* fileptr;
    size_t filesize;

    // used to detect changes
    u32 file_generation;
    std::filesystem::file_time_type last_loaded_time;

    /**
     * @brief Load a file into the object, from path
     *
     * @param path that path to the file
     * @return true
     * @return false
     */
    bool load_file(const std::string& path) {
        FILE* fp = std::fopen(path.c_str(), "r");
        if (fp == NULL)
            return false;

        // find file size
        std::fseek(fp, 0, SEEK_END);
        filesize = std::ftell(fp);
        std::fseek(fp, 0, SEEK_SET);

        // load data
        fileptr = malloc(filesize);
        if (fread(fileptr, filesize, 1, fp) == 0)
            return false;

        return true;
    }
};

}

//
// LiveFile
//

LiveFile::LiveFile(const std::string& path) {

    m_id = LiveFileManager::checkout_file(path).expect("Handle me");
    m_last_checked_generation = 0;
}

LiveFile::~LiveFile() { LiveFileManager::return_file(m_id); }

LiveFile::LiveFile(const LiveFile& f) {
    LiveFileManager::checkout_file(f.m_id);
    m_id = f.m_id;
}

const LiveFile& LiveFile::operator=(const LiveFile& f) {
    LiveFileManager::checkout_file(f.m_id);
    return f;
}

u64 LiveFile::retrieve_generation(void) {
    return LiveFileManager::get_file_generation(m_id);
}

bool LiveFile::was_reloaded(void) {
    u64 current_generation = retrieve_generation();
    if (current_generation > m_last_checked_generation) {
        m_last_checked_generation = current_generation;
        return true;
    }
    return false;
}

bool LiveFile::was_reloaded_no_reset(void) {
    u64 current_generation = retrieve_generation();
    if (current_generation > m_last_checked_generation) {
        return true;
    }
    return false;
}

void LiveFile::reset_was_reloaded(void) {
    m_last_checked_generation = retrieve_generation();
}

const void* LiveFile::retrieve_contents(void) {
    return LiveFileManager::access_file(m_id).expect(
        "Invalid file or handle deletion");
}

std::size_t LiveFile::retrieve_size(void) {
    return LiveFileManager::get_file_size(m_id).expect(
        "Invalid uid, or handle deletion");
}

//
// LiveManager Implementation
//

void* LiveFileManager::thread_main() {

    std::cout << "Initialized live file" << std::endl;

    while (!LiveFileManager::m_stop_thread) {
        LiveFileManager::check_for_reloads();
        std::this_thread::sleep_for(std::chrono::seconds(1)); // FIXME hardcoded
    }

    return NULL;
}

void __attribute__((constructor)) LiveFileManager::init_manager(void) {

    m_testthread = new std::thread(thread_main);
}

void __attribute__((destructor)) LiveFileManager::terminate_manager(void) {
    LiveFileManager::m_stop_thread = true;
    std::cout << "terminate called\n";

    m_testthread->join();

    LiveFileManager::m_files.clear();
}

Result<const void*> LiveFileManager::access_file(const uid_t id) {
    return Result(m_files.at(id)->get_contents());
}

Result<std::size_t> LiveFileManager::get_file_size(const uid_t id) {
    try {
        return Result(m_files.at(id)->get_size());
    } catch (std::out_of_range e) {
        return Result<size_t>(ResultErr());
    }
}

void LiveFileManager::check_for_reloads(void) {
    for (auto& f : LiveFileManager::m_files) {

        // reload file if it was modified
        if (f.second->needs_reload()) {
            std::cout << "Change detected with " << f.second->get_path()
                      << std::endl; // FIXME debug
            f.second->reload_file();
        }
    }
}

Result<uid_t> LiveFileManager::checkout_file(const std::string& path) {

    // check if the file has already been loaded
    try {
        const uid_t id = m_path_registry.at(path);
        m_files.at(id)->checkout_file();
        return id;
    } catch (std::out_of_range e) {

        uid_t id = Utils::UID::generate();

        m_files
            .insert(std::pair(
                id, std::unique_ptr<LoadedFile>{ new LoadedFile(path, id) }))
            .first->second->checkout_file();
        m_path_registry.insert(std::pair(path, id));
        return id;
    }
}

Result<uid_t> LiveFileManager::checkout_file(const uid_t id) {
    m_files.at(id)->checkout_file();
    return id;
}

void LiveFileManager::return_file(const uid_t id) {
    auto& f = m_files.at(id);
    f->return_file();

    // delete the file if unused
    if (!f->in_use()) {
        m_files.erase(id);
    }
}

u64 LiveFileManager::get_file_generation(const uid_t id) {
    return m_files.at(id)->get_generation();
}

} // inline Core
} // Engine
} // VGED