/**
 * @file   GLUniformBuffer.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   25. Januar 2014
 *
 * @brief  Contains the definition of GLUniformBuffer.
 */

#ifndef GLUNIFORMBUFFER_H
#define GLUNIFORMBUFFER_H

#include "../../main.h"

namespace cgu {

    class ShaderBufferBindingPoints;

    /**
     * @brief  Represents uniform buffers.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   25. Januar 2014
     */
    class GLUniformBuffer
    {
    private:
        /** Deleted copy constructor. */
        GLUniformBuffer(const GLUniformBuffer& orig) : bindingPoints(orig.bindingPoints) {};
        /** Deleted copy assignment operator. */
        GLUniformBuffer& operator=(const GLUniformBuffer&) { return *this; };

    public:
        GLUniformBuffer(const std::string& name, unsigned int size, ShaderBufferBindingPoints& bindings);
        virtual ~GLUniformBuffer();

        void UploadData(unsigned int offset, unsigned int size, const void* data) const;
        void BindBuffer() const;

    private:
        /** holds the uniform buffer object. */
        GLuint ubo;
        /** Holds the size of the buffer. */
        unsigned int bufferSize;
        /** holds the uniform buffer binding points. */
        ShaderBufferBindingPoints& bindingPoints;
        /** holds the buffer binding point. */
        GLuint bindingPoint;

    };
}

#endif /* GLUNIFORMBUFFER_H */
