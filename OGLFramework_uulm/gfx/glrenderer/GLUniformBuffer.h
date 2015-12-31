/**
 * @file   GLUniformBuffer.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.01.25
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
     * @date   2014.01.25
     */
    class GLUniformBuffer
    {
    public:
        GLUniformBuffer(const std::string& name, unsigned int size, ShaderBufferBindingPoints* bindings);
        GLUniformBuffer(const GLUniformBuffer&);
        GLUniformBuffer& operator=(GLUniformBuffer);
        GLUniformBuffer(GLUniformBuffer&&);
        GLUniformBuffer& operator=(GLUniformBuffer&&);
        virtual ~GLUniformBuffer();

        void UploadData(unsigned int offset, unsigned int size, const void* data) const;
        void BindBuffer() const;
        ShaderBufferBindingPoints* GetBindingPoints() const { return bindingPoints; }
        const std::string& GetUBOName() const { return uboName; }

    private:
        /** holds the uniform buffer object. */
        GLuint ubo;
        /** Holds the size of the buffer. */
        unsigned int bufferSize;
        /** holds the uniform buffer binding points. */
        ShaderBufferBindingPoints* bindingPoints;
        /** holds the buffer binding point. */
        GLuint bindingPoint;
        /** Holds the uniform buffers name. */
        std::string uboName;

    };
}

#endif /* GLUNIFORMBUFFER_H */
