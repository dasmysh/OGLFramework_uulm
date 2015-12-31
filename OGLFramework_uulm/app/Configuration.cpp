/**
 * @file   Configuration.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2013.12.18
 * @ingroup win
 *
 * @brief  Implementation of the configuration class for windows systems.
 */

#include "Configuration.h"

namespace cgu {

    Configuration::Configuration() :
        fullscreen(false),
        backbufferBits(32),
        windowLeft(0),
        windowTop(0),
        windowWidth(800),
        windowHeight(600),
        useSRGB(false),
        pauseOnKillFocus(false),
        resourceBase("resources"),
        useCUDA(true),
        cudaDevice(-1)
    {
    }

    Configuration::~Configuration()
    {
    }

    /**
     * Stream method used by boost serialization of the configuration.
     * @ingroup win
     * @param os the output stream
     * @param config the configuration to serialize
     * @return the output stream
     */
    std::ostream & operator<<(std::ostream &os, const Configuration &config)
    {
        return os << config.fullscreen << config.backbufferBits << config.windowLeft << config.windowTop
            << config.windowWidth << config.windowHeight << config.useSRGB << config.pauseOnKillFocus
            << config.resourceBase << config.useCUDA << config.cudaDevice;
    }
}
