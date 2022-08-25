#pragma once

#include "core/debug.hpp"
#include <cstdint>
#include <string>

namespace VGED {
namespace Engine {
inline namespace Core {
inline namespace Types {


using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using usize = std::size_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using isize = std::ptrdiff_t;

using f32 = float;
using f64 = double;

template<typename T>
class Result {

    public:

    /**
     * @brief Initialize a result as a failure;
     *
     */
    Result();

    Result(T& value);

    /**
     * @brief return a failure
     *
     * @return constexpr Result
     */
    static inline constexpr Result FAILURE() { return Result{}; }

    /**
     * @brief return a successful result and its value;
     *
     * @param val
     * @return constexpr Result
     */
    static inline constexpr Result SUCCESS(T& val) { return Result{val}; }

    /**
     * @brief get the value stored in the result. If the result failed, it will
     * return nullptr.
     *
     * @return T&
     */
    inline T& operator*() {
        if (!success_) return nullptr;
        return value_;
    }
    inline void operator=(const T& t) { value_ = t; success_ = true; };

    /**
     * @brief Expect the result to be successful. If not, it throws an exception.
     *
     * @return T& the successful result;
     */
    inline T& expect(const std::string err) {
        if (success_) return value_;
        THROW(err);
    };

    /**
     * @brief Check if the result was successful;
     *
     * @return true
     * @return false
     */
    inline bool was_success(void) { return success_; }

    private:
    T value_;
    bool success_;
};

} // Types
} // Core
} // Engine
} // VGED