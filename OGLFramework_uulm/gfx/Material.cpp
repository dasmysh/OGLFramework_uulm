/**
 * @file   Material.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   31. Dezember 2013
 *
 * @brief  Contains the implementation of Material methods.
 */

#include "Material.h"

namespace cgu {
    Material::Material() :
        ambient(0.0f, 0.0f, 0.0f),
        diffuse(0.0f, 0.0f, 0.0f),
        specular(0.0f, 0.0f, 0.0f),
        alpha(1.0f),
        minOrientedAlpha(0.0f),
        N_s(1.0f),
        N_i(1.0f),
        diffuseTex(nullptr)
    {
    }

    Material::~Material()
    {
    }
}
