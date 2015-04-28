/**
 * @file    cudaLogger.h
 * @author  Sebastian Maisch
 * @date    2014.06.04
 *
 * @brief Logging interface to g2log from cuda files (host code).
 */

#ifndef CUDALOGGER_H
#define CUDALOGGER_H

#include <string>
#include <ostream>

namespace cudaLog {
    std::wostream& log(const std::string& file, const int line);
}

#endif /* CUDALOGGER_H */
