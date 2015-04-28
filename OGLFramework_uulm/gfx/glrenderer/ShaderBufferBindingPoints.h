/**
 * @file   ShaderBufferBindingPoints.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   26. Januar 2014
 *
 * @brief  Contains the definition of ShaderBufferBindingPoints.
 */

#ifndef UNIFORMBUFFERBINDINGPOINTS_H
#define UNIFORMBUFFERBINDINGPOINTS_H

#include "main.h"

/**
 * @brief Manages the binding points for shader buffers (uniform buffers, textures, ...).
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   26. Januar 2014
 */
class ShaderBufferBindingPoints
{
private:
    /** Deleted copy constructor. */
    ShaderBufferBindingPoints(const ShaderBufferBindingPoints&) {};
    /** Deleted copy assignment operator. */
    ShaderBufferBindingPoints& operator=(const ShaderBufferBindingPoints&) { return *this; };

public:
    ShaderBufferBindingPoints() : bindingPoints(), nextBindingPoint(0) {};
    virtual ~ShaderBufferBindingPoints() {};

    GLuint GetBindingPoint(const std::string& name);

private:
    /** holds map that maps uniform buffer names to binding points. */
    std::unordered_map<std::string, GLuint> bindingPoints;
    /** holds the next free binding point. */
    unsigned int nextBindingPoint;

};

#endif /* UNIFORMBUFFERBINDINGPOINTS_H */
