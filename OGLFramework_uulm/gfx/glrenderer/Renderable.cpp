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

    /** Copy constructor. */
    Renderable::Renderable(const Renderable& orig)
    {
    }

    /** Move constructor. */
    Renderable::Renderable(Renderable&& orig)
    {
    }

    /** Copy assignment operator. */
    Renderable& Renderable::operator=(const Renderable& orig)
    {
        return *this;
    }

    /** Move assignment operator. */
    Renderable& Renderable::operator=(Renderable&& orig)
    {
        return *this;
    }

    /** Destructor. */
    Renderable::~Renderable()
    {
    }
}
