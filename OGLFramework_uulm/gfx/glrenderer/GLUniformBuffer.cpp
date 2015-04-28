/**
 * @file   GLUniformBuffer.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   25. Januar 2014
 *
 * @brief  Contains the implementation of GLUniformBuffer.
 */

#include "GLUniformBuffer.h"

/**
 * Constructor.
 * @param name the name of the uniform buffer.
 * @param size the size of the uniform buffer
 * @param bindings the binding points used to bind the buffer to
 */
GLUniformBuffer::GLUniformBuffer(const std::string& name, unsigned int size,
        ShaderBufferBindingPoints& bindings) :
ubo(0),
bufferSize(size),
bindingPoints(bindings),
bindingPoint(bindingPoints.GetBindingPoint(name))
{
    OGL_CALL(glGenBuffers, 1, &ubo);
    OGL_CALL(glBindBuffer, GL_UNIFORM_BUFFER, ubo);
    OGL_CALL(glBufferData, GL_UNIFORM_BUFFER, size, nullptr, GL_STREAM_DRAW);
    OGL_CALL(glBindBuffer, GL_UNIFORM_BUFFER, 0);
    BindBuffer();
}

/** Destructor. */
GLUniformBuffer::~GLUniformBuffer()
{
    if (ubo != 0) {
        OGL_CALL(glDeleteBuffers, 1, &ubo);
        ubo = 0;
    }
}

/**
 * Upload data to the uniform buffer.
 * @param offset the offset into the buffer to store the data
 * @param size the size of the data
 * @param data the data to store in the buffer
 */
void GLUniformBuffer::UploadData(unsigned int offset, unsigned int size, const void* data)
{
    assert((offset + size) <= bufferSize);
    OGL_CALL(glBindBuffer, GL_UNIFORM_BUFFER, ubo);
    OGL_CALL(glBufferSubData, GL_UNIFORM_BUFFER, offset, size, data);
    OGL_CALL(glBindBuffer, GL_UNIFORM_BUFFER, 0);
}

void GLUniformBuffer::BindBuffer()
{

    OGL_CALL(glBindBufferRange, GL_UNIFORM_BUFFER, bindingPoint, ubo, 0, bufferSize);
}
