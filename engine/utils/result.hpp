#pragma once

#include "core/debug.hpp"
#include <optional>
#include <stdexcept>
#include <string>
#include <cstdarg>

struct ResultErr {

    /**
     * @brief Construct a new Result Err object
     *
     * @param err the error messege. supports printf arguments
     * @param ...
     */
    ResultErr(const char* err, ...) : message() {
        std::va_list args;
        va_start(args, err);

        char buf[1024];
        vsnprintf(buf, sizeof(buf), err, args);
        message.assign(buf);

        va_end(args);
    }

    ResultErr() { message = "Some dumbass didn't write an error messege"; }

    operator std::string() { return message; }

    std::string message = {};
};

template <typename T> class Result {
private:
    std::optional<T> v = {};
    std::string m = {};

public:
    Result(T&& value) : v{ std::move(value) }, m{ "" } {}
    Result(T const& value) : v{ value }, m{ "" } {}
    Result(std::optional<T>&& opt)
        : v{ std::move(opt) }, m{ opt.has_value() ? ""
                                                  : "default error message" } {}
    Result(std::optional<T> const& opt)
        : v{ opt }, m{ opt.has_value() ? "" : "default error message" } {}
    Result(ResultErr const& err) : v{ std::nullopt }, m{ err.message } {}
    Result(std::string_view message) : v{ std::nullopt }, m{ message } {}

    bool is_ok() const { return v.has_value(); }
    bool is_err() const { return !v.has_value(); }

    T const& value() const { return v.value(); }

    T& value() {
        if (!v.has_value()) {
            throw std::runtime_error(
                (!m.empty() ? m : "tried getting value of empty Result"));
        }
        return v.value();
    }

    T expect(ResultErr errmsg) {
        if (v.has_value())
            return v.value();
        else
            THROW(errmsg.message);
    }

    const std::string& message() const { return m; }
    operator bool() const { return v.has_value(); }
    bool operator!() const { return !v.has_value(); }
    operator T() { return v.value(); }
};
