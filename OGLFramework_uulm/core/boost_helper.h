/**
 * @file   boost_helper.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   24. Februar 2014
 *
 * @brief  Contains the definition of boost helpers.
 */

#ifndef BOOST_HELPER_H
#define BOOST_HELPER_H

#include "../main.h"
#include <boost/property_tree/ptree.hpp>

/**
 * Helper class for float parsing in xml files.
 * This makes '.' as well as ',' to be used as decimal separator in floats.
 */
class FloatTranslator
{
public:
    /** The internal xml type. */
    typedef std::string internal_type;
    /** The type to convert to. */
    typedef float external_type;

    boost::optional<external_type> get_value(internal_type const &v);
    boost::optional<internal_type> put_value(external_type const& v);
};

class ColorTranslator
{
public:
    /** The internal xml type. */
    typedef std::string internal_type;
    /** The type to convert to. */
    typedef glm::vec3 external_type;

    boost::optional<external_type> get_value(internal_type const &v);
    boost::optional<internal_type> put_value(external_type const& v);
};
#endif /* BOOST_HELPER_H */
