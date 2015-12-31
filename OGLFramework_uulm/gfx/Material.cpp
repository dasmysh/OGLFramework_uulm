/**
 * @file   Material.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2013.12.31
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
        diffuseTex(nullptr),
        bumpTex(nullptr),
        bumpMultiplier(1.0f)
    {
    }

    /** Default copy constructor. */
    Material::Material(const Material&) = default;
    /** Default copy assignment operator. */
    Material& Material::operator=(const Material&) = default;

    /** Default move constructor. */
    Material::Material(Material&& rhs) :
        ambient(std::move(rhs.ambient)),
        diffuse(std::move(rhs.diffuse)),
        specular(std::move(rhs.specular)),
        alpha(std::move(rhs.alpha)),
        minOrientedAlpha(std::move(rhs.minOrientedAlpha)),
        N_s(std::move(rhs.N_s)),
        N_i(std::move(rhs.N_i)),
        diffuseTex(std::move(rhs.diffuseTex)),
        bumpTex(std::move(rhs.bumpTex)),
        bumpMultiplier(std::move(rhs.bumpMultiplier))
    {
    }

    /** Default move assignment operator. */
    Material& Material::operator=(Material&& rhs)
    {
        this->~Material();
        ambient = std::move(rhs.ambient);
        diffuse = std::move(rhs.diffuse);
        specular = std::move(rhs.specular);
        alpha = std::move(rhs.alpha);
        minOrientedAlpha = std::move(rhs.minOrientedAlpha);
        N_s = std::move(rhs.N_s);
        N_i = std::move(rhs.N_i);
        diffuseTex = std::move(rhs.diffuseTex);
        bumpTex = std::move(rhs.bumpTex);
        bumpMultiplier = std::move(rhs.bumpMultiplier);
        return *this;
    }

    Material::~Material() = default;
}
