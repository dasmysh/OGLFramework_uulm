/**
 * @file   oglErrorHandling.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.06.15
 *
 * @brief  Implementation of OpenGL call error handlers.
 */

#include "main.h"

/**
* Log an OpenGL error.
* @param file the file the error producing function was called from
* @param line the line the error producing function was called in
* @param func the function the error producing function was called from
* @param glfn the OpenGL functions name
*/
void log_ogl_err(const std::string& file, int line, const std::string& func, const std::string& glfn)
{
    auto err = glGetError();
    auto doprint = true;
    std::string errorStr;
    switch (err)
    {
    case GL_INVALID_ENUM:
        errorStr = "GL_INVALID_ENUM";
        break;
    case GL_INVALID_VALUE:
        errorStr = "GL_INVALID_VALUE";
        break;
    case GL_INVALID_OPERATION:
        errorStr = "GL_INVALID_OPERATION";
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        errorStr = "GL_INVALID_FRAMEBUFFER_OPERATION";
        break;
    case GL_OUT_OF_MEMORY:
        errorStr = "GL_OUT_OF_MEMORY";
        break;
    case GL_STACK_UNDERFLOW:
        errorStr = "GL_STACK_UNDERFLOW";
        break;
    case GL_STACK_OVERFLOW:
        errorStr = "GL_STACK_OVERFLOW";
        break;
    default:
        doprint = false;
        break;
    }

    if (doprint) {
        std::stringstream call;
        // call << file << "(" << line << "): " << func << "-> " << glfn << " Returned error: " << errorStr;
        call << glfn << " Returned error: " << errorStr;

        auto simpleStr = call.str();
        std::wstring wideStr(simpleStr.begin(), simpleStr.end());
        if (ERROR >= APPLICATION_LOG_LEVEL) g2::internal::LogMessage(file, line, func, "ERROR  ").messageStream() << wideStr;
        // G2_LOG_ERROR.messageStream() << wideStr;
    }
}

/**
* Helper function for the ogl_call_fc_params template with 0 arguments.
* @return empty string
*/
std::string ogl_call_fn_params()
{
    return "";
}

/**
* Helper function for the ogl_call_fc_params template with a single std::nullptr_t argument.
* @return address of the nullptr ("0x00")
*/
std::string ogl_call_fn_params(std::nullptr_t)
{
    return "0x00";
}

/**
* Helper function for the ogl_call_fc_params template with a single char pointer argument.
* @param ptr the char pointer
* @return address of the char pointer as string
*/
std::string ogl_call_fn_params(char* ptr)
{
    std::stringstream conv;
    conv << static_cast<void*>(ptr);
    return conv.str();
}
