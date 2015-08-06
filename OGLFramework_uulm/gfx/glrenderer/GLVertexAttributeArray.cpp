/**
 * @file   GLVertexAttributeArray.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   16. Februar 2014
 *
 * @brief  Contains the implementation of GLVertexAttributeArray.
 */

#include "GLVertexAttributeArray.h"

namespace cgu {

    /**
     * Constructor.
     * @param vertexBuffer the vertex buffer used to create the attributes.
     * @param indexBuffer the indexbuffer to bind
     */
    GLVertexAttributeArray::GLVertexAttributeArray(GLuint vertexBuffer, GLuint indexBuffer) :
        vao(0),
        i_buffer(indexBuffer),
        v_buffer(vertexBuffer),
        v_desc()
    {
        OGL_CALL(glGenVertexArrays, 1, &vao);
    }

    GLVertexAttributeArray::GLVertexAttributeArray(GLVertexAttributeArray&& orig) :
        vao(orig.vao),
        i_buffer(orig.i_buffer),
        v_buffer(orig.v_buffer),
        v_desc(std::move(orig.v_desc))
    {
        orig.vao = 0;
        orig.i_buffer = 0;
        orig.v_buffer = 0;
    }

    GLVertexAttributeArray& GLVertexAttributeArray::operator =(GLVertexAttributeArray&& orig)
    {
        if (this != &orig) {
            vao = orig.vao;
            i_buffer = orig.i_buffer;
            v_buffer = orig.v_buffer;
            v_desc = std::move(orig.v_desc);
            orig.vao = 0;
            orig.i_buffer = 0;
            orig.v_buffer = 0;
        }
        return *this;
    }

    GLVertexAttributeArray::~GLVertexAttributeArray()
    {
        if (vao != 0) {
            OGL_CALL(glDeleteVertexArrays, 1, &vao);
        }
    }

    /** Disables all vertex attributes in the array. */
    void GLVertexAttributeArray::DisableAttributes()
    {
        OGL_CALL(glBindVertexArray, vao);
        OGL_CALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0);

        for (const vertex_attribute_desc& desc : v_desc) {
            if (desc.location->iBinding > 0) {
                OGL_CALL(glDisableVertexAttribArray, desc.location->iBinding);
            }
        }

        OGL_CALL(glBindVertexArray, 0);
    }

    /** Initializes the vertex attribute setup. */
    void GLVertexAttributeArray::StartAttributeSetup()
    {
        OGL_CALL(glBindVertexArray, vao);
        OGL_CALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    /** Ends the vertex attribute setup. */
    void GLVertexAttributeArray::EndAttributeSetup()
    {
        OGL_CALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, i_buffer);
        OGL_CALL(glBindVertexArray, 0);
        OGL_CALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    /** Enables the vertex attribute array. */
    void GLVertexAttributeArray::EnableVertexAttributeArray() const
    {
        OGL_CALL(glBindVertexArray, vao);
    }

    /** Disables the vertex attribute array. */
    void GLVertexAttributeArray::DisableVertexAttributeArray() const
    {
        OGL_CALL(glBindVertexArray, 0);
    }

    /**
     * Updates all vertex attributes.
     * This is used when BindingLocation changes after a shader recompile.
     */
    void GLVertexAttributeArray::UpdateVertexAttributes()
    {
        OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, v_buffer);
        OGL_CALL(glBindVertexArray, vao);
        OGL_CALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, i_buffer);

        for (const vertex_attribute_desc& desc : v_desc) {
            if (desc.location->iBinding > 0) {
                OGL_CALL(glEnableVertexAttribArray, desc.location->iBinding);
                switch (desc.shaderType)
                {
                case VAShaderType::FLOAT:
                    OGL_CALL(glVertexAttribPointer, desc.location->iBinding, desc.size,
                        desc.type, desc.normalized, desc.stride,
                        (static_cast<char*> (nullptr)) + desc.offset);
                    break;
                case VAShaderType::INTEGER:
                    OGL_CALL(glVertexAttribIPointer, desc.location->iBinding, desc.size,
                        desc.type, desc.stride, (static_cast<char*> (nullptr)) + desc.offset);
                    break;
                case VAShaderType::DOUBLE:
                    OGL_CALL(glVertexAttribLPointer, desc.location->iBinding, desc.size,
                        desc.type, desc.stride, (static_cast<char*> (nullptr)) + desc.offset);
                    break;
                }
            }
        }

        OGL_CALL(glBindVertexArray, 0);
        OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);
    }

    /**
     * Adds a new floating point attribute to the vertex.
     * @param location the binding location
     * @param size the number of components in the attribute
     * @param type the type in which the data is stored in the vertex buffer
     * @param normalized whether the attributes values will be normalized to (0..1)
     * @param stride the difference in bytes between of this attributes
     * @param offset the offset into the buffer to the first of this attributes
     */
    void GLVertexAttributeArray::AddVertexAttribute(BindingLocation location, int size, GLenum type,
        GLboolean normalized, GLsizei stride, unsigned int offset)
    {
        vertex_attribute_desc desc;
        desc.shaderType = VAShaderType::FLOAT;
        desc.location = location;
        desc.size = size;
        desc.type = type;
        desc.normalized = normalized;
        desc.stride = stride;
        desc.offset = offset;
        v_desc.push_back(std::move(desc));
        OGL_CALL(glEnableVertexAttribArray, location->iBinding);
        OGL_CALL(glVertexAttribPointer, location->iBinding, size, type, normalized, stride,
            (static_cast<char*> (nullptr)) + offset);
    }

    /**
     * Adds a new integer attribute to the vertex.
     * @param location the binding location
     * @param size the number of components in the attribute
     * @param type the type in which the data is stored in the vertex buffer
     * @param stride the difference in bytes between of this attributes
     * @param offset the offset into the buffer to the first of this attributes
     */
    void GLVertexAttributeArray::AddVertexAttributeI(BindingLocation location, int size, GLenum type,
        GLsizei stride, unsigned int offset)
    {
        vertex_attribute_desc desc;
        desc.shaderType = VAShaderType::INTEGER;
        desc.location = location;
        desc.size = size;
        desc.type = type;
        desc.stride = stride;
        desc.offset = offset;
        v_desc.push_back(std::move(desc));
        OGL_CALL(glEnableVertexAttribArray, location->iBinding);
        OGL_CALL(glVertexAttribIPointer, location->iBinding, size, type, stride,
            (static_cast<char*> (nullptr)) + offset);
    }

    /**
     * Adds a new double attribute to the vertex.
     * @param location the binding location
     * @param size the number of components in the attribute
     * @param type the type in which the data is stored in the vertex buffer
     * @param stride the difference in bytes between of this attributes
     * @param offset the offset into the buffer to the first of this attributes
     */
    void GLVertexAttributeArray::AddVertexAttributeL(BindingLocation location, int size, GLenum type,
        GLsizei stride, unsigned int offset)
    {
        vertex_attribute_desc desc;
        desc.shaderType = VAShaderType::DOUBLE;
        desc.location = location;
        desc.size = size;
        desc.type = type;
        desc.stride = stride;
        desc.offset = offset;
        v_desc.push_back(std::move(desc));
        OGL_CALL(glEnableVertexAttribArray, location->iBinding);
        OGL_CALL(glVertexAttribLPointer, location->iBinding, size, type, stride,
            (static_cast<char*> (nullptr)) + offset);
    }
}
