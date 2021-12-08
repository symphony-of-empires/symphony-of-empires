#include <cstdarg>
#include <cstdio>
#include <mutex>

#include <GL/glew.h>
#include <cstdint>
#include <GL/gl.h>

#include "print.hpp"

static std::mutex print_mutex;
static bool allow_debug = true;

void print_enable_debug(void) {
    allow_debug = true;
}

void print_disable_debug(void) {
    allow_debug = true;
}

#if defined windows
#   include <windows.h>
#   include <WinCon.h>
#endif

void print_error(const char* str, ...) {
    print_mutex.lock();

    va_list args;
    va_start(args, str);

#if defined unix
    printf("\e[36m[INFO]\e[0m ");
#else
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY | BACKGROUND_BLUE);
    printf("* <error> ");
#endif

    vprintf(str, args);
    printf("\n");

#if defined windows
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY | BACKGROUND_BLUE);
#endif

    va_end(args);

    print_mutex.unlock();
}

void print_info(const char* str, ...) {
    if(!allow_debug) return;

    print_mutex.lock();

    va_list args;
    va_start(args, str);

#if defined unix
    printf("\e[36m[INFO]\e[0m ");
#else
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY | BACKGROUND_BLUE);
    printf("* <debug> ");
#endif

    vprintf(str, args);
    printf("\n");


#if defined windows
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY | BACKGROUND_BLUE);
#endif

    va_end(args);

    print_mutex.unlock();
}

// Callback function for printing debug statements
void GLAPIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length,
    const GLchar* msg, const void* data)
{
    std::string _source;
    std::string _type;
    std::string _severity;

    switch(source) {
    case GL_DEBUG_SOURCE_API:
        _source = "API";
        break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = "WINDOW SYSTEM";
        break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = "SHADER COMPILER";
        break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = "THIRD PARTY";
        break;

    case GL_DEBUG_SOURCE_APPLICATION:
        _source = "APPLICATION";
        break;

    case GL_DEBUG_SOURCE_OTHER:
        _source = "UNKNOWN";
        break;

    default:
        _source = "UNKNOWN";
        break;
    }

    switch(type) {
    case GL_DEBUG_TYPE_ERROR:
        _type = "ERROR";
        break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        _type = "DEPRECATED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        _type = "UDEFINED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_PORTABILITY:
        _type = "PORTABILITY";
        break;

    case GL_DEBUG_TYPE_PERFORMANCE:
        _type = "PERFORMANCE";
        break;

    case GL_DEBUG_TYPE_OTHER:
        _type = "OTHER";
        break;

    case GL_DEBUG_TYPE_MARKER:
        _type = "MARKER";
        break;

    default:
        _type = "UNKNOWN";
        break;
    }

    switch(severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        _severity = "HIGH";
        break;

    case GL_DEBUG_SEVERITY_MEDIUM:
        _severity = "MEDIUM";
        break;

    case GL_DEBUG_SEVERITY_LOW:
        _severity = "LOW";
        break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
        _severity = "NOTIFICATION";
        return;
        break;

    default:
        _severity = "UNKNOWN";
        break;
    }

    printf("%d: %s of %s severity, raised from %s: %s\n",
        id, _type.c_str(), _severity.c_str(), _source.c_str(), msg);
}