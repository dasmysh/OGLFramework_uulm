/**
 * @file    Renderable.cpp
 * @author  Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date    DATE
 *
 * @brief   2014.03.30
 */

#include "Renderable.h"

namespace cgu {

    /** Default constructor. */
    Renderable::Renderable()
    {
    }

    /** Default move constructor. */
    Renderable::Renderable(const Renderable&&)
    {
    }

    /** Default move assignment operator. */
    Renderable& Renderable::operator=(const Renderable&&)
    {
        return *this;
    }

    /** Destructor. */
    Renderable::~Renderable()
    {
    };
}
