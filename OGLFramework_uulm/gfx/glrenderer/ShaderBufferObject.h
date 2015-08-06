/**
 * @file   ShaderBufferObject.h
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.06.15
 *
 * @brief  Declaration of an open gl shader buffer object.
 */

#ifndef SHADERBUFFEROBJECT_H
#define SHADERBUFFEROBJECT_H

#include "main.h"

namespace cgu {
    
    class ShaderBufferBindingPoints;

    /**
     * @brief Wrapper to access OpenGL shader storage buffer objects (SSBOs).
     */
    class ShaderBufferObject
    {
    public:
        ShaderBufferObject(const std::string& name, ShaderBufferBindingPoints& bindings);
        ShaderBufferObject(const std::string& name, unsigned int size, ShaderBufferBindingPoints& bindings);
        ShaderBufferObject(const ShaderBufferObject& orig) = delete;
        ShaderBufferObject& operator=(const ShaderBufferObject& orig) = delete;
        ShaderBufferObject(ShaderBufferObject&& orig) = delete;
        ShaderBufferObject& operator=(ShaderBufferObject&& orig) = delete;
        ~ShaderBufferObject();

        void BindBuffer() const;
        void UploadData(unsigned int offset, unsigned int size, const void* data) const;

    private:
        /** holds the buffer id. */
        GLuint ssbo;
        /** Holds the current size of the buffer. */
        unsigned int bufferSize;
        /** holds the shader buffer objects binding points. */
        ShaderBufferBindingPoints& bindingPoints;
        /** holds the buffer binding point. */
        GLuint bindingPoint;
    };
}

#endif /* SHADERBUFFEROBJECT_H */
