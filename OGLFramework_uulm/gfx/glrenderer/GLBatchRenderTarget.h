/**
 * @file   GLBatchRenderTarget.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   16. Januar 2014
 *
 * @brief  Contains the definition of GLBatchRenderTarget.
 */

#ifndef BATCHRENDERTARGET_H
#define BATCHRENDERTARGET_H

#include <vector>

namespace cgu {

    class GLVertexAttributeArray;
    class GLRenderTarget;
    class GPUProgram;
    /** The type for a list of vertex arrays. */
    typedef std::vector<GLVertexAttributeArray*> VertexAttributeBindings;
    class Renderable;
    class ScreenText;
    class GUIRenderable;
    class Font;

    /** Flags representing which buffers to clear. */
    enum class ClearFlags : unsigned int
    {
        CF_RenderTarget = 0x01, ///< Clear the render target.
        CF_Depth = 0x02, ///< Clear the depth buffer.
        CF_Stencil = 0x04 ///< Clear the stencil buffer.
    };

    /**
     * @brief  A GLBatchRenderTarget is used for rendering into a render target without ensuring the render targets
     * frame buffer is set in every call.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   16. Januar 2014
     */
    class GLBatchRenderTarget
    {
    private:
        /** Deleted copy constructor. */
        GLBatchRenderTarget(const GLBatchRenderTarget& orig) = delete;
        /** Deleted copy assignment operator. */
        GLBatchRenderTarget& operator=(const GLBatchRenderTarget&) = delete;

    public:
        GLBatchRenderTarget(GLRenderTarget& renderTarget);
        virtual ~GLBatchRenderTarget();

        void Clear(unsigned int clflags, const float color[4],
            float depth, unsigned int stencil);
        void Draw(const GPUProgram* program, const Renderable* renderable,
            const VertexAttributeBindings* bindings);
        void UseFont(Font* font);
        void DrawScreenText(ScreenText* text);
        void DrawGUIGeometry(GUIRenderable* guiElement);
        // void DrawGUIText(GUIRenderable* guiElement);
        void EnableAlphaBlending();
        void DisableAlphaBlending();

    private:
        GLRenderTarget& target;
    };
}

#endif /* BATCHRENDERTARGET_H */