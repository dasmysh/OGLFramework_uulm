/**
 * @file   GUISlider.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   21. Februar 2014
 *
 * @brief  Contains the definition of GUISlider.
 */

#ifndef GUISLIDER_H
#define GUISLIDER_H

/**
 * @brief
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   21. Februar 2014
 */
class GUISlider
{
public:
    GUISlider();
    virtual ~GUISlider();
    /** Deleted copy constructor. */
    GUISlider(const GUISlider& orig) = delete;
    /** Deleted copy assignment operator. */
    GUISlider& operator=(const GUISlider& orig) = delete;
    /** Default move constructor. */
    GUISlider(GUISlider&& orig) = default;
    /** Default move assignment operator. */
    GUISlider& operator=(GUISlider&& orig) = default;
private:

};

#endif /* GUISLIDER_H */
