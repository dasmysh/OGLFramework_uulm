/**
 * @file   Material.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2013.12.31
 *
 * @brief  Contains the material class
 */

#ifndef MATERIAL_H
#define MATERIAL_H

#include "main.h"

namespace cgu {

    class GLTexture2D;

    /**
     * @brief  Class describing materials.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   2013.12.31
     */
    class Material
    {
    public:
        Material();
        Material(const Material&);
        Material& operator=(const Material&);
        Material(Material&&);
        Material& operator=(Material&&);
        ~Material();

        /** Holds the materials ambient color. */
        glm::vec3 ambient;
        /** Holds the materials diffuse color. */
        glm::vec3 diffuse;
        /** Holds the materials specular color. */
        glm::vec3 specular;
        /** Holds the materials alpha value. */
        float alpha;
        /** Holds the materials minimum oriented alpha value. */
        float minOrientedAlpha;
        /** Holds the materials specular exponent. */
        float N_s;
        /** Holds the materials index of refraction. */
        float N_i;
        /** Holds the materials diffuse texture. */
        const GLTexture2D* diffuseTex;
        /** Holds the materials bump texture. */
        const GLTexture2D* bumpTex;
        /** Holds the materials bump multiplier. */
        float bumpMultiplier;
    };
}

#endif /* MATERIAL_H */
