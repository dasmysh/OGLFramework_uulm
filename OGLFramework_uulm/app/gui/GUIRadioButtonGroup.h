/**
 * @file   GUIRadioButtonGroup.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   21. Februar 2014
 *
 * @brief  Contains the definition of GUIRadioButtonGroup.
 */

#ifndef GUIRADIOBUTTONGROUP_H
#define GUIRADIOBUTTONGROUP_H

/**
 * @brief
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   21. Februar 2014
 */
class GUIRadioButtonGroup
{
public:
    GUIRadioButtonGroup();
    virtual ~GUIRadioButtonGroup();
    /** Deleted copy constructor. */
    GUIRadioButtonGroup(const GUIRadioButtonGroup& orig) = delete;
    /** Deleted copy assignment operator. */
    GUIRadioButtonGroup& operator=(const GUIRadioButtonGroup& orig) = delete;
    /** Default move constructor. */
    GUIRadioButtonGroup(GUIRadioButtonGroup&& orig) = default;
    /** Default move assignment operator. */
    GUIRadioButtonGroup& operator=(GUIRadioButtonGroup&& orig) = default;
private:

};

#endif /* GUIRADIOBUTTONGROUP_H */
