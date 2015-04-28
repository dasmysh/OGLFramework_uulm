/**
 * @file   cudaLogger.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.06.04
 *
 * @brief  Contains the implementation of cuda logging functions.
 */

#include "core/cudaLogger.h"
#include "core/g2log/g2log.h"

namespace cudaLog {

    std::wostream& log(const std::string& file, const int line)
    {
        return g2::internal::LogMessage(file, line, "", "ERROR").messageStream();
    }
}