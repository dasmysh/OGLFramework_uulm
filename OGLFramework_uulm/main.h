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

#pragma warning(push, 3)
#include <string>
#include <vector>
#include <deque>
#include <list>
#include <map>
#include <cassert>
#include <exception>
#include <stdexcept>
#include <unordered_map>
#include <cmath>
#include <memory>
#include <iterator>
#pragma warning(pop)

#pragma warning(disable : 4201)
#include <glm/glm.hpp>
#pragma warning(default : 4201)
#include <GL/glew.h>

#include "core/regex_helper.h"

class ApplicationBase;

#include "app/BaseGLWindow.h"

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#pragma warning(push, 3)
#include <Windows.h>
#include <GL/wglew.h>
#pragma warning(pop)

#include "app/GLWindow.h"

#include "core/g2logWrapper.h"
#include "constants.h"

#define OGL_SCALL(call) call()

#define OGL_CALL(call, ...) call(__VA_ARGS__)

#include "core/Resource.h"
#include "core/ResourceManager.h"

class Material;

#endif // main_h__
