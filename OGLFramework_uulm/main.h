/**
 * @file    main.h
 * @author  Sebastian Maisch
 * @date    22.06.2012
 *
 * @brief Includes common headers and defines common templates.
 */

#ifndef main_h__
#define main_h__

#pragma warning(disable: 4514 4711 4996)

#if defined ( _MSC_VER )
#define __func__ __FUNCTION__
#endif

#pragma warning(push, 3)
#include <array>
#include <cassert>
#include <cmath>
#include <deque>
#include <exception>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#pragma warning(pop)

#pragma warning(disable : 4201)
#include <glm/glm.hpp>
#pragma warning(default : 4201)
#include <GL/glew.h>

#include "core/regex_helper.h"

class ApplicationBase;

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#pragma warning(push, 3)
#include <Windows.h>
#include <GL/wglew.h>
#pragma warning(pop)

#include "core/g2logWrapper.h"
#include "constants.h"

#ifdef _OGL_DEBUG_MSGS

void log_ogl_err(const std::string& file, int line, const std::string& func, const std::string& glfn);

std::string ogl_call_fn_params();

std::string ogl_call_fn_params(std::nullptr_t);
std::string ogl_call_fn_params(char* ptr);

template <typename T>
std::string ogl_call_fn_params(const T& arg)
{
    std::stringstream conv;
    const char* type;
    type = typeid (arg).name();
    conv << "(" << type << ") " << arg;
    return conv.str();
}

template <typename T, typename ...Rest>
std::string ogl_call_fn_params(const T& first, const Rest&... rest)
{
    std::stringstream conv;
    conv << ogl_call_fn_params(first);
    return conv.str() + ", " + ogl_call_fn_params(rest...);
}

template <typename ...Args>
void log_ogl_call(const std::string& file, int line, const std::string& func,
    const std::string& glfn, const Args&... args)
{
    std::stringstream call;
    call << glfn << "(" << ogl_call_fn_params(args...) << ");";

    std::string simpleStr = call.str();
    std::wstring wideStr(simpleStr.begin(), simpleStr.end());
    if (DEBUG >= APPLICATION_LOG_LEVEL) g2::internal::LogMessage(file, line, func, "DEBUG  ").messageStream() << wideStr;
}

#define OGL_FUNCTION_STR(call) #call
#define OGL_SCALL(call) call(); \
log_ogl_call(__FILE__, __LINE__, __func__, OGL_FUNCTION_STR(call)); \
log_ogl_err(__FILE__, __LINE__, __func__, OGL_FUNCTION_STR(call))
#define OGL_CALL(call, ...) call(__VA_ARGS__); \
log_ogl_call(__FILE__, __LINE__, __func__, OGL_FUNCTION_STR(call), __VA_ARGS__); \
log_ogl_err(__FILE__, __LINE__, __func__, OGL_FUNCTION_STR(call))
#else
/**
* Debug macro for calling an OpenGL function without parameters. This will log all calls in debug mode.
* @param call the OpenGL function.
*/
#define OGL_SCALL(call)  call()

/**
* Debug macro for calling an OpenGL function with parameters. This will log all calls in debug mode.
* @param call the OpenGL function.
*/
#define OGL_CALL(call, ...)  call(__VA_ARGS__)
#endif

#include "core/Resource.h"
#include "core/ResourceManager.h"

namespace cgu {
    class GLVertexAttributeArray;
    class Material;
    /** The type for a list of vertex arrays. */
    typedef std::vector<GLVertexAttributeArray*> VertexAttributeBindings;
}

static const unsigned int VK_NUMRETURN  = 0x100;
static const unsigned int VK_NUMDECIMAL = 0x101;
static const unsigned int VK_NUMDELETE  = 0x102;

static const unsigned int VK_MOD_SHIFT = 0x01;
static const unsigned int VK_MOD_CTRL = 0x02;
static const unsigned int VK_MOD_MENU = 0x04;
static const unsigned int VK_MOD_ALTGR = 0x08;

static const unsigned int MB_LEFT = 0x01;
static const unsigned int MB_RGHT = 0x02;
static const unsigned int MB_MIDD = 0x04;
static const unsigned int MB_BTN4 = 0x08;
static const unsigned int MB_BTN5 = 0x10;

#endif // main_h__
