/**
 * @file   primitives.h
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.07.28
 *
 * @brief  Definition of geometric primitives.
 */

#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <array>
#include <glm/glm.hpp>

namespace cguMath {
    template<typename real> using Seg2 = std::array<glm::tvec2<real, glm::highp>, 2>;
    template<typename real> using Seg3 = std::array<glm::tvec3<real, glm::highp>, 2>;

    template<typename real> using Tri2 = std::array<glm::tvec2<real, glm::highp>, 3>;
    template<typename real> using Tri3 = std::array<glm::tvec3<real, glm::highp>, 3>;
}

#endif // PRIMITIVES_H
