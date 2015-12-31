/**
 * @file   ShaderBufferBindingPoints.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.01.26
 *
 * @brief  Contains the definition of ShaderBufferBindingPoints.
 */

#ifndef UNIFORMBUFFERBINDINGPOINTS_H
#define UNIFORMBUFFERBINDINGPOINTS_H

#include "main.h"

namespace cgu {
    /**
     * @brief Manages the binding points for shader buffers (uniform buffers, textures, ...).
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   2014.01.26
     */
    class ShaderBufferBindingPoints
    {
        /** Deleted copy constructor. */
        ShaderBufferBindingPoints(const ShaderBufferBindingPoints&) = delete;
        /** Deleted copy assignment operator. */
        ShaderBufferBindingPoints& operator=(const ShaderBufferBindingPoints&) = delete;

    public:
        ShaderBufferBindingPoints() : bindingPoints(), nextBindingPoint(0) {};
        ~ShaderBufferBindingPoints();

        GLuint GetBindingPoint(const std::string& name);

    private:
        /** holds map that maps uniform buffer names to binding points. */
        std::unordered_map<std::string, GLuint> bindingPoints;
        /** holds the next free binding point. */
        unsigned int nextBindingPoint;

    };
}

#endif /* UNIFORMBUFFERBINDINGPOINTS_H */
