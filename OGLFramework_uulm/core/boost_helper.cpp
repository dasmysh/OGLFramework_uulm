/**
 * @file   boost_helper.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.02.24
 *
 * @brief  Contains the implementation of boost helpers.
 */

#include "boost_helper.h"
#include <iomanip>

/**
 * Returns the float value of a string.
 * @param v the string
 * @return the strings float value
 */
boost::optional<FloatTranslator::external_type> FloatTranslator::get_value(internal_type const &v)
{
    auto tmpstr = v;
    std::replace(tmpstr.begin(), tmpstr.end(), ',', '.');
    return static_cast<float>(std::atof(tmpstr.c_str()));
}

/**
 * Converts a float into a string.
 * @param v the float value
 * @return the converted string
 */
boost::optional<FloatTranslator::internal_type> FloatTranslator::put_value(external_type const& v)
{
    std::stringstream conv;
    conv << v;
    return conv.str();
}

boost::optional<ColorTranslator::external_type> ColorTranslator::get_value(const internal_type& v)
{
    unsigned int r = strtoul(v.substr(1, 2).c_str(), nullptr, 16);
    unsigned int g = strtoul(v.substr(3, 2).c_str(), nullptr, 16);
    unsigned int b = strtoul(v.substr(5, 2).c_str(), nullptr, 16);
    return glm::vec3(static_cast<float> (r) / 255.0f, static_cast<float> (g) / 255.0f, static_cast<float> (b) / 255.0f);
}

boost::optional<ColorTranslator::internal_type> ColorTranslator::put_value(const external_type& v)
{
    std::stringstream conv;
    auto r = static_cast<unsigned int> (v.r * 255.0f);
    auto g = static_cast<unsigned int> (v.g * 255.0f);
    auto b = static_cast<unsigned int> (v.b * 255.0f);
    conv << "#" << std::hex << std::setfill('0') << std::setw(6) << ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
    return conv.str();
}
