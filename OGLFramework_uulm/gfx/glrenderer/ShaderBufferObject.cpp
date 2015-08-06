/**
 * @file   ShaderBufferObject.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.06.15
 *
 * @brief  Implementation of shader buffer objects.
 */

#include "ShaderBufferObject.h"
#include "ShaderBufferBindingPoints.h"

namespace cgu {
    /**
    * Constructor.
    * @param name the name of the shader buffer object.
    * @param size the size of the buffer it is to be created with
    * @param bindings the binding points used to bind the buffer to
    */
    ShaderBufferObject::ShaderBufferObject(const std::string& name, unsigned int size, ShaderBufferBindingPoints& bindings) :
        ssbo(0),
        bufferSize(size),
        bindingPoints(bindings),
        bindingPoint(bindingPoints.GetBindingPoint(name))
    {
        OGL_CALL(glGenBuffers, 1, &ssbo);

        if (bufferSize > 0) {
            OGL_CALL(glBindBuffer, GL_SHADER_STORAGE_BUFFER, ssbo);
            OGL_CALL(glBufferData, GL_SHADER_STORAGE_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);
            OGL_CALL(glBindBuffer, GL_SHADER_STORAGE_BUFFER, 0);
            BindBuffer();
        }
    }

    /**
    * Constructor.
    * @param name the name of the shader buffer object.
    * @param bindings the binding points used to bind the buffer to
    */
    ShaderBufferObject::ShaderBufferObject(const std::string& name, ShaderBufferBindingPoints& bindings) :
        ShaderBufferObject(name, 0, bindings)
    {
    }

    ShaderBufferObject::~ShaderBufferObject()
    {
        if (ssbo != 0) {
            OGL_CALL(glDeleteBuffers, 1, &ssbo);
            ssbo = 0;
        }
    }

    void ShaderBufferObject::BindBuffer() const
    {
        OGL_CALL(glBindBufferBase, GL_SHADER_STORAGE_BUFFER, bindingPoint, ssbo);
    }

    void ShaderBufferObject::UploadData(unsigned int offset, unsigned int size, const void* data) const
    {
        OGL_CALL(glBindBuffer, GL_SHADER_STORAGE_BUFFER, ssbo);
        if (offset + size > bufferSize) {
            std::vector<int8_t> tmp(offset);
            OGL_CALL(glGetBufferSubData, GL_SHADER_STORAGE_BUFFER, 0, offset, tmp.data());
            OGL_CALL(glBufferData, GL_SHADER_STORAGE_BUFFER, offset + size, nullptr, GL_DYNAMIC_DRAW);
            OGL_CALL(glBufferSubData, GL_SHADER_STORAGE_BUFFER, 0, offset, tmp.data());
        }

        OGL_CALL(glBufferSubData, GL_SHADER_STORAGE_BUFFER, offset, size, data);
        OGL_CALL(glBindBuffer, GL_SHADER_STORAGE_BUFFER, 0);
    }
}
