/**
 * @file   Material.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   31. Dezember 2013
 *
 * @brief  Contains the material class
 */

#ifndef MATERIAL_H
#define MATERIAL_H

#include "main.h"
#include "glrenderer/GLTexture2D.h"

/**
 * @brief  Class describing materials.
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   31. Dezember 2013
 */
class Material
{
public:
    Material();

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
private:

};

#endif /* MATERIAL_H */
