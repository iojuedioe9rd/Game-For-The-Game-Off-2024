#pragma once
#include "Vertex/Core/Base.h"
#include "Vertex/Core/Logger.h"
#include <filesystem>

#ifdef VX_ENABLE_ASSERTS

// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
#define VX_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { VX##type##ERROR(msg, __VA_ARGS__); VX_DEBUGBREAK(); } }
#define VX_INTERNAL_ASSERT_WITH_MSG(type, check, ...) VX_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define VX_INTERNAL_ASSERT_NO_MSG(type, check) VX_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", VX_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)
#define VX_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define VX_INTERNAL_ASSERT_GET_MACRO(...) VX_EXPAND_MACRO( VX_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, VX_INTERNAL_ASSERT_WITH_MSG, VX_INTERNAL_ASSERT_NO_MSG) )
// Currently accepts at least the condition and one additional parameter (the message) being optional
#define VX_ASSERT(...) VX_EXPAND_MACRO( VX_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
#define VX_CORE_ASSERT(...) VX_EXPAND_MACRO( VX_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
#include "Vertex/Core/ErrorBox.h"
#include <sstream>
#include <string>

void VERTEX_API Assert_INTERNAL_Close();

// Lambda-based message formatting macro
#define FORMAT_MESSAGE(...) \
    ([](auto&&... args) { \
        std::ostringstream stream; \
        (stream << ... << std::forward<decltype(args)>(args)); \
        return stream.str(); \
    }(__VA_ARGS__))

// Assertions when asserts are disabled
#define VX_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { \
    if (!(check)) { \
        std::string formatted_msg = FORMAT_MESSAGE(msg, __VA_ARGS__); \
        if (type == "_CORE_") { \
            ErrorBox("Core assertion failed: " + formatted_msg); \
            Assert_INTERNAL_Close(); \
        } else { \
            ErrorBox("Assertion failed: " + formatted_msg); \
        } \
    } \
}

#define VX_INTERNAL_ASSERT_WITH_MSG(type, check, ...) VX_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define VX_INTERNAL_ASSERT_NO_MSG(type, check) VX_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed.", VX_STRINGIFY_MACRO(check))

#define VX_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define VX_INTERNAL_ASSERT_GET_MACRO(...) VX_EXPAND_MACRO(VX_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, VX_INTERNAL_ASSERT_WITH_MSG, VX_INTERNAL_ASSERT_NO_MSG))

#define VX_ASSERT(...) 
#define VX_CORE_ASSERT(...) VX_EXPAND_MACRO(VX_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)("_CORE_", __VA_ARGS__))

#endif