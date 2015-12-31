/**
 * @file   ShaderBufferBindingPoints.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.01.26
 *
 * @brief  Contains the implementation of ShaderBufferBindingPoints.
 */

#include "ShaderBufferBindingPoints.h"

namespace cgu {

    ShaderBufferBindingPoints::~ShaderBufferBindingPoints()
    {
    }

    /**
     * Returns the binding point for a shader buffer with the given name.
     * @param name the name of the buffer.
     * @return the global binding point for this name
     */
    GLuint ShaderBufferBindingPoints::GetBindingPoint(const std::string& name)
    {
        try {
            return bindingPoints.at(name);
        }
        catch (std::out_of_range e) {
            bindingPoints[name] = nextBindingPoint;
            return nextBindingPoint++;
        }
    }
}
