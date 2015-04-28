/**
 * @file   Renderable.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   19. Januar 2014
 *
 * @brief  Contains the definition of Renderable.
 */

#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "main.h"
class GLVertexAttributeArray;
class GPUProgram;
/** The type for a list of vertex arrays. */
typedef std::vector<GLVertexAttributeArray*> VertexAttributeBindings;

/**
 * @brief  Interface for all renderable objects.
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   19. Januar 2014
 */
class Renderable
{
private:
    /** Deleted copy constructor. */
    Renderable(const Renderable&) {};
    /** Deleted copy assignment operator. */
    Renderable& operator=(const Renderable&) { return *this; };

public:
    Renderable();
    Renderable(const Renderable&& orig);
    Renderable& operator=(const Renderable&& orig);
    virtual ~Renderable();

    /**
     * Draws the renderable object using the vertex attribute bindings given.
     * The VertexAttributeBindings need to be created by the same object that uses them.
     * @param bindings the vertex attributes to use
     */
    virtual void Draw(const VertexAttributeBindings& bindings) const = 0;
    /**
     * Generates the vertex attribute bindings for this Renderable and the given GPUProgram.
     * @param program the program to generate the bindings for
     * @param bindings the bindings to fill
     */
    virtual void FillVertexAttributeBindings(GPUProgram& program,
            VertexAttributeBindings& bindings) const = 0;

private:
};

#endif /* RENDERABLE_H */
