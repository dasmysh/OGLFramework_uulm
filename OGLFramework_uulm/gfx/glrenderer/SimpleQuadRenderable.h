/**
 * @file   SimpleQuadRenderable.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   28. Februar 2014
 *
 * @brief  Contains the definition of SimpleQuadRenderable.
 */

#ifndef SIMPLEQUADRENDERABLE_H
#define SIMPLEQUADRENDERABLE_H

#include "main.h"

/**
 * @brief
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   28. Februar 2014
 */
class SimpleQuadRenderable
{
private:
    /** Deleted copy constructor. */
    SimpleQuadRenderable(const SimpleQuadRenderable&) {};
    /** Deleted copy assignment operator. */
    SimpleQuadRenderable& operator=(const SimpleQuadRenderable&) { return *this; };

public:
    SimpleQuadRenderable(const glm::vec2* vertexData, GPUProgram* prog);
    virtual ~SimpleQuadRenderable();
    /** Default move constructor. */
    SimpleQuadRenderable(SimpleQuadRenderable&& orig);
    /** Default move assignment operator. */
    SimpleQuadRenderable& operator=(SimpleQuadRenderable&& orig);

    void Render();

private:
    /** Holds the vertex buffer object to use. */
    GLuint vBuffer;
    /** Holds the index buffer object name. */
    GLuint iBuffer;
    /** Holds the rendering gpu program. */
    GPUProgram* program;
    /** Holds the vertex attribute bindings for the shader. */
    std::unique_ptr<GLVertexAttributeArray> attribBind;

};

#endif /* SIMPLEQUADRENDERABLE_H */
