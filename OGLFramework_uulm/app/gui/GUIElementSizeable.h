/**
 * @file   GUIElementSizeable.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   27. Februar 2014
 *
 * @brief  Contains the definition of GUIElementSizeable.
 */

#ifndef GUIELEMENTSIZEABLE_H
#define GUIELEMENTSIZEABLE_H

#include "GUIElement.h"
#include "gfx/glrenderer/GUIRenderable.h"
#include "gui_info.h"

/**
 * @brief  Abstract GUI element that is used for elements that have a size
 * determinded by some text or by a length variing per instance.<br/>
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   27. Februar 2014
 */
class GUIElementSizeable : public GUIElement
{
public:
    GUIElementSizeable(const glm::vec3& relPos);
    virtual ~GUIElementSizeable() = default;
    /** Deleted copy constructor. */
    GUIElementSizeable(const GUIElementSizeable& orig) = delete;
    /** Deleted copy assignment operator. */
    GUIElementSizeable& operator=(const GUIElementSizeable& orig) = delete;
    /** Default move constructor. */
    GUIElementSizeable(GUIElementSizeable&& orig) = default;
    /** Default move assignment operator. */
    GUIElementSizeable& operator=(GUIElementSizeable&& orig) = default;


    virtual void RenderGeometry(GLBatchRenderTarget& rt) override;

protected:
    void InitializeSize(const glm::vec2& compSize, const guiinfo::element_info& left,
            const guiinfo::element_info& right, GPUProgram* guiProgram);

private:
    /** Holds the gui renderable for unfocused rendering. */
    std::unique_ptr<GUIRenderable> renderable = {};
    /** Holds the gui renderable for focused rendering. */
    std::unique_ptr<GUIRenderable> renderableFocused = {};

};

#endif /* GUIELEMENTSIZEABLE_H */
