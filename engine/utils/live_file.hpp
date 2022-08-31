/**
 * @file live_file.hpp
 * @author Kael Johnston (wkaelj@gmail.com)
 * @brief Automatically reload files when they are changed
 * @version 0.1
 * @date 2022-08-30
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <list>
#include <string>
#include "vged.hpp"
#include <utils/uid.hpp>

namespace VGED {
namespace Engine {
inline namespace Core {

using namespace Utils::UID;

/**
 * @brief A handle to a file managed by the system
 *
 */
class LiveFile {

    struct PointLight {
        struct {float x, y, z;}  position;
        float _align1_;

        struct {float x, y, z;} color;
        float intensity;

        struct {float x, y, z;} ambient;
        float _align2_;

        struct {float x, y, z;} diffuse;
        float linear;

        struct {float x, y, z;} specular;
        float quadratic;
};

public:
    LiveFile(const std::string& path);
    LiveFile(const LiveFile& f);
    ~LiveFile();

    /**
     * @brief get the contents of the file
     *
     * @return void*
     */
    const void* operator*(void) { return retrieve_contents(); }
    bool operator==(LiveFile&& f) { return m_id == f.m_id; }

    const LiveFile& operator=(const LiveFile& f);

    /**
     * @brief Get the generation of the file
     *
     * @return u64
     */
    u64 retrieve_generation(void);

    /**
     * @brief Get the last checked generation of the file
     *
     * @return u64
     */
    inline u64 get_checked_generation(void) {
        return m_last_checked_generation;
    }

    /**
     * @brief Check if a file was reloaded since the last call to this function
     *
     * @return true
     * @return false
     */
    bool was_reloaded(void);

    /**
     * @brief Check if the file was reloaded, but it will not stop returning
     * true once it is called.
     *
     * @return true
     * @return false
     */
    bool was_reloaded_no_reset(void);

    /**
     * @brief Reset the internal state storing whether the file was reloaded
     *
     */
    void reset_was_reloaded(void);

    /**
     * @brief Get the contents of the file
     *
     * @return const void*
     */
    void* retrieve_contents(void);

    /**
     * @brief Get the size object
     *
     * @return std::size_t
     */
    std::size_t retrieve_size(void);

private:
    uid_t m_id;

    u64 m_last_checked_generation;
};

}
}
}