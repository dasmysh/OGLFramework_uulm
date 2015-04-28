/**
 * @file   regex_helper.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   9. Januar 2014
 *
 * @brief  Contains definitions helping with regex.
 */

#ifndef REGEX_HELPER_H
#define REGEX_HELPER_H

/** Contains definitions helping with regex. */
namespace regex_help
{
    /** Match string for a single float (no exponents allowed). */
    const std::string flt = "([-+]?[0-9]*\\.?[0-9]+)";
    /** Match string for two whitespace separated floats (no exponents allowed). */
    const std::string flt2 = flt + "\\s+" + flt;
    /** Match string for 3 whitespace separated floats (no exponents allowed). */
    const std::string flt3 = flt2 + "\\s+" + flt;
    /** Match string for 4 whitespace separated floats (no exponents allowed). */
    const std::string flt4 = flt3 + "\\s+" + flt;
    /** Match string for an integer. */
    const std::string integ = "([-+]?[0-9]+)";
}

#endif /* REGEX_HELPER_H */
