/* VGED Copyright (c) 2022 VGED ORGANIZATION
   https://github.com/VGED-ORGANIZATION/VGED-ENGINE
   License: GPL-3

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation files
   (the "Software"), to deal in the Software without restriction,
   including without limitation the rights to use, copy, modify, merge,
   publish, distribute, sublicense, and/or sell copies of the Software,
   and to permit persons to whom the Software is furnished to do so,
   subject to the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/

#pragma once

#include <iostream>
#include "log/log.hpp"

#undef far
#undef near
#undef CopyFile
#undef CreateDirectory
#undef CreateWindow

#define ASSERT(x) if (!(x)) std::cout << " (ASSERT on line number " << __LINE__ << " in file " << __FILE__ << ")" << std::endl;
#define memberSize(type, member) sizeof(((type *)0)->member)
#define BIT(x) (1 << (x))

#define LOG_CORE_TRACE(...)     VGED::Log::getLogger()->trace(__VA_ARGS__)
#define LOG_CORE_INFO(...)      VGED::Log::getLogger()->info(__VA_ARGS__)
#define LOG_CORE_WARN(...)      VGED::Log::getLogger()->warn(__VA_ARGS__)
#define LOG_CORE_ERROR(...)     VGED::Log::getLogger()->error(__VA_ARGS__)
#define LOG_CORE_CRITICAL(...)  VGED::Log::getLogger()->critical(__VA_ARGS__)

#define LOG_APP_TRACE(...)      VGED::Log::getAppLogger()->trace(__VA_ARGS__)
#define LOG_APP_INFO(...)       VGED::Log::getAppLogger()->info(__VA_ARGS__)
#define LOG_APP_WARN(...)       VGED::Log::getAppLogger()->warn(__VA_ARGS__)
#define LOG_APP_ERROR(...)      VGED::Log::getAppLogger()->error(__VA_ARGS__)
#define LOG_APP_CRITICAL(...)   VGED::Log::getAppLogger()->critical(__VA_ARGS__)

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