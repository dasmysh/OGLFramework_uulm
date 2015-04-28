/**
 * @file   GUICheckbox.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   21. Februar 2014
 *
 * @brief  Contains the definition of GUICheckbox.
 */

#ifndef GUICHECKBOX_H
#define GUICHECKBOX_H

/**
 * @brief
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   21. Februar 2014
 */
class GUICheckbox
{
public:
    GUICheckbox();
    virtual ~GUICheckbox();
    /** Deleted copy constructor. */
    GUICheckbox(const GUICheckbox& orig) = delete;
    /** Deleted copy assignment operator. */
    GUICheckbox& operator=(const GUICheckbox& orig) = delete;
    /** Default move constructor. */
    GUICheckbox(GUICheckbox&& orig) = default;
    /** Default move assignment operator. */
    GUICheckbox& operator=(GUICheckbox&& orig) = default;
private:

};

#endif /* GUICHECKBOX_H */
