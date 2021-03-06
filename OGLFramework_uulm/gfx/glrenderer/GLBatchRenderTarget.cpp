/**
 * @file   GLBatchRenderTarget.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.01.16
 *
 * @brief  Contains the implementation of GLBatchRenderTarget.
 */

#include "GLBatchRenderTarget.h"
#include "main.h"
#include "ScreenText.h"

namespace cgu {

    /**
     * Constructor.
     * @param renderTarget the render target to use
     */
    GLBatchRenderTarget::GLBatchRenderTarget(GLRenderTarget& renderTarget) :
        target(renderTarget)
    {
    }

    GLBatchRenderTarget::~GLBatchRenderTarget()
    {
    }

    /**
     * Clears the backbuffer.
     * @param clflags the clear flags
     * @param color the new color (only used if clflags contains ClearFlags::CF_RenderTarget)
     * @param depth the depth to set the depth buffer to (only used if clflags contains ClearFlags::CF_Depth)
     * @param stencil the value to set the stencil buffer to (only used if clflags contains ClearFlags::CF_Stencil)
     */
    // ReSharper disable once CppMemberFunctionMayBeStatic
    // ReSharper disable once CppMemberFunctionMayBeConst
    void GLBatchRenderTarget::Clear(unsigned int clflags, const float color[], float depth, unsigned int stencil)
    {
        GLbitfield clearFlags = 0;
        if (clflags & static_cast<unsigned int>(ClearFlags::CF_RenderTarget)) {
            OGL_CALL(glClearColor, color[0], color[1], color[2], color[3]);
            clearFlags |= GL_COLOR_BUFFER_BIT;
        }
        if (clflags & static_cast<unsigned int>(ClearFlags::CF_Depth)) {
            OGL_CALL(glClearDepth, static_cast<double> (depth));
            clearFlags |= GL_DEPTH_BUFFER_BIT;
        }
        if (clflags & static_cast<unsigned int>(ClearFlags::CF_Stencil)) {
            OGL_CALL(glClearStencil, stencil);
            clearFlags |= GL_STENCIL_BUFFER_BIT;
        }

        OGL_CALL(glClear, clearFlags);
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    // ReSharper disable once CppMemberFunctionMayBeConst
    void GLBatchRenderTarget::DrawScreenText(ScreenText* text)
    {
        text->DrawMultiple();
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    // ReSharper disable once CppMemberFunctionMayBeConst
    void GLBatchRenderTarget::UseFont(Font*)
    {
        // TODO: implement
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    // ReSharper disable once CppMemberFunctionMayBeConst
    /** Enables alpha blending on this target. */
    void GLBatchRenderTarget::EnableAlphaBlending()
    {
        OGL_CALL(glEnable, GL_BLEND);
        OGL_CALL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    // ReSharper disable once CppMemberFunctionMayBeConst
    /** Disables alpha blending on this target. */
    void GLBatchRenderTarget::DisableAlphaBlending()
    {
        OGL_CALL(glDisable, GL_BLEND);
    }
}
