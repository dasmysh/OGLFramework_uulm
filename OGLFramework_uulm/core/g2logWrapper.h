/**
 * @file    g2logWrapper.h
 * @author  Sebastian Maisch
 * @date    04.06.2014
 *
 * @brief Wrapper for g2log macros.
 */

#ifndef G2LOGWRAPPER_H
#define G2LOGWRAPPER_H

#include "core/g2log/g2log.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "core/g2log/g2logworker.h"

#undef LOG
#undef LOG_IF
#undef LOGF
#undef LOGF_IF

#ifdef _DEBUG

/** The applications log level. */
static const int APPLICATION_LOG_LEVEL = GL_DEBUG;

#else

/** The applications log level. */
static const int APPLICATION_LOG_LEVEL = INFO;

#endif // _DEBUG

// redefine g2logs log defines for conditional logging by log level
#define LOG(level)  \
    if (level >= APPLICATION_LOG_LEVEL) G2_LOG_##level.messageStream()

#define LOG_IF(level, boolean_expression)  \
  if(level >= APPLICATION_LOG_LEVEL && true == boolean_expression)  \
     G2_LOG_##level.messageStream()

#define LOGF(level, printf_like_message, ...)  \
    if (level >= APPLICATION_LOG_LEVEL) G2_LOGF_##level.messageSave(printf_like_message, ##__VA_ARGS__)

#define LOGF_IF(level,boolean_expression, printf_like_message, ...)  \
    if(level >= APPLICATION_LOG_LEVEL && true == boolean_expression)  \
        G2_LOG_##level.messageSave(printf_like_message, ##__VA_ARGS__)

#endif /* G2LOGWRAPPER_H */
