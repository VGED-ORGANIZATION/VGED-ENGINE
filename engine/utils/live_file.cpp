#include "live_file.hpp"

#include <ctime>
#include <cstdio>

namespace VGED {
namespace Engine {
inline namespace Core {

//
// LiveFileManager Decleration
//

namespace LiveFileManager {

struct LoadedFile;

std::list<LoadedFile> m_files;

bool m_stop_thread = false; // used to stop the thread
pthread_t m_testthread;

void* thread_main(void* arg);

void init_manager(void) __attribute__((constructor));
void terminate_manager(void) __attribute((destructor));

Result<void*> access_file(const uid_t id);
Result<std::size_t> get_file_size(const uid_t id);

// iterate through files and check if any need to be reloaded
void check_for_reloads(void);
// reload the contents of a file
void reload_file(LoadedFile& file);

// handle file io
void* load_file(const std::string& path, size_t& size);
void unload_file(void* mem);

bool was_file_modified(const std::string& path, time_t loaded_time);

u64 get_file_generation(const uid_t id);

// check out and return files, so the manager knows when they can be
// released
Result<uid_t> checkout_file(const std::string& path);
Result<uid_t> checkout_file(const uid_t id);

void return_file(const uid_t id);
void return_file(const std::string& path);

struct LoadedFile {

    bool operator==(const LoadedFile& lf) { return id == lf.id; }

    /**
     * @brief The number of references to file file in use
     */
    u32 reference_count;
    /**
     * @brief The files unique identifier
     */
    uid_t id;
    /**
     * @brief The path to the file
     */
    std::string path;
    /**
     * @brief The actual file data
     */
    void* fileptr;
    /**
     * @brief The size of the loaded file
     */
    size_t filesize;
    /**
     * @brief The last time the file was reloaded
     */
    time_t last_loaded_time;
    /**
     * @brief Store the number of times the file has been reloaded.
     */
    u32 file_generation;
};

}

//
// LiveFile
//

LiveFile::LiveFile(const std::string& path) {

    m_id = LiveFileManager::checkout_file(path).expect("Handle me");
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

void* LiveFile::retrieve_contents(void) {
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

void* thread_main(void* arg) {

    while (!LiveFileManager::m_stop_thread) {
        LiveFileManager::check_for_reloads();
        sleep(1);
    }

    return NULL;
}

void init_manager(void) {

    pthread_create(&LiveFileManager::m_testthread, NULL,
                   &LiveFileManager::thread_main, NULL);
}
void terminate_manager(void) {
    LiveFileManager::m_stop_thread = true;

    pthread_join(LiveFileManager::m_testthread, NULL);

    LiveFileManager::m_files.clear();
}

time_t get_file_modified_time(const std::string& path) {

    time_t file_modified_time;
#ifdef OS_POSIX
#include <sys/stat.h>

    struct stat buf;
    if (stat(path.c_str(), &buf))
        THROW("Must handle file deletion");
    file_modified_time = buf.st_mtim.tv_sec;
#elif OS_WINDOWS
#include <fileapi.h>
#include <errhandlingapi.h>

    HANDLE f = CreateFileA(path.c_str(), GENERIC_READ, 0, 0, 0, OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL, 0);
    LPWORD

    FILETIME buf;
    GetFileTime(f, NULL, NULL, &buf);

    if (GetLastError() == ERROR_FILE_NOT_FOUND) {
        THROW("Must handle file deletion");
    }

    CloseHandle(f);

    // convert windows api filetime to unix time
    file_modified_time = (uint32_t)((buf - 116444736000000000l) / 10000000l);

#else
#error Unsupported os
#endif
    return file_modified_time;
}

Result<void*> LiveFileManager::access_file(const uid_t id) {

    // find matching object in map
    for (auto f : LiveFileManager::m_files) {
        if (f.id == id)
            return Result<void*>::SUCCESS(f.fileptr);
    }

    VGED_ENGINE_ERROR("File with id %lu has not been loaded", id);
    return Result<void*>::FAILURE();
}

Result<std::size_t> get_file_size(const uid_t id) {
    // find matching object in map
    for (auto f : LiveFileManager::m_files) {
        if (f.id == id)
            return Result<void*>::SUCCESS(f.filesize);
    }

    return Result<size_t>::FAILURE();
}

void LiveFileManager::check_for_reloads(void) {

    time_t time = std::time(NULL);

    for (auto f : LiveFileManager::m_files) {

        time_t file_modified_time = get_file_modified_time(f.path);

        // reload file if it was modified
        if (was_file_modified(f.path, f.last_loaded_time)) {
            LiveFileManager::reload_file(f);
        }
    }
}

void LiveFileManager::reload_file(LoadedFile& f) {
    LiveFileManager::unload_file(f.fileptr);
    LiveFileManager::load_file(f.path, f.filesize);
    ++f.file_generation;
    f.last_loaded_time = std::time(NULL);
}

void* LiveFileManager::load_file(const std::string& path, size_t& size) {

    FILE* fp = fopen(path.c_str(), "r");

    if (fp == NULL) {
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    void* mem = malloc(size);

    fread(mem, size, 1, fp);

    fclose(fp);

    return mem;
}

void LiveFileManager::unload_file(void* mem) { free(mem); }

bool LiveFileManager::was_file_modified(const std::string& path,
                                        time_t loaded_time) {
    if (loaded_time < get_file_modified_time(path))
        return true;
    else
        return false;
}

Result<uid_t> checkout_file(const std::string& path) {

    for (auto f : LiveFileManager::m_files) {
        if (f.path.compare(path) == 0) {
            ++f.reference_count;
            return f.id;
        }
    }

    size_t size;
    void* fp = LiveFileManager::load_file(path, size);
    if (fp == NULL)
        return Result<uid_t>::FAILURE();

    LiveFileManager::LoadedFile file_object{
        .reference_count = 1,
        .id = Utils::UID::generate(),
        .path = path,
        .fileptr = fp,
        .filesize = size,
        .last_loaded_time = time(NULL),
        .file_generation = 1,
    };

    LiveFileManager::m_files.push_back(file_object);

    return Result<uid_t>::SUCCESS(file_object.id);
}

Result<uid_t> checkout_file(const uid_t id) {
    for (auto f : LiveFileManager::m_files) {
        if (f.id == id) {
            ++f.reference_count;
            return f.id;
        }
    }

    THROW("Invalid UID used when checkout out a file");
    return Result<uid_t>::FAILURE();
}

void return_file(const uid_t id) {
    for (auto f : LiveFileManager::m_files) {
        if (f.id == id) {
            --f.reference_count;

            // if there are 0 references, unload the file
            if (f.reference_count <= 0) {
                LiveFileManager::unload_file(f.fileptr);
                LiveFileManager::m_files.remove(f);
            }
        }
    }
}

void return_file(const std::string& path) {
    for (auto f : LiveFileManager::m_files) {
        if (f.path.compare(path) == 0) {
            --f.reference_count;

            // if there are 0 references, unload the file
            if (f.reference_count <= 0) {
                LiveFileManager::unload_file(f.fileptr);
                LiveFileManager::m_files.remove(f);
            }
        }
    }
}

u64 get_file_generation(const uid_t id) {

    for (auto f : LiveFileManager::m_files) {
        if (f.id == id)
            return f.file_generation;
    }

    THROW("Invalid uid when checking for file");
}

} // inline Core
} // Engine
} // VGED