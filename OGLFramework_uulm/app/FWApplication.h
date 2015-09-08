/**
 * @file   FWApplication.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.01.21
 *
 * @brief  Contains the definition of FWApplication.
 */

#ifndef FWAPPLICATION_H
#define FWAPPLICATION_H

#include "ApplicationBase.h"
#include "gfx/glrenderer/MeshRenderable.h"
#include "gfx/glrenderer/GPUProgram.h"
#include "gfx/glrenderer/GLUniformBuffer.h"
#include "gfx/glrenderer/Font.h"
#include "gfx/glrenderer/ScreenText.h"
#include "gfx/OBJMesh.h"
#include "scene/ArcballRotatedSceneRenderable.h"

namespace cguFrameworkApp {

    /**
     * @brief Implementation of the framework application.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   2014.01.21
     */
    class FWApplication : public cgu::ApplicationBase
    {
    public:
        FWApplication(cgu::GLWindow& window);
        virtual ~FWApplication();

        virtual bool HandleKeyboard(unsigned int vkCode, bool bKeyDown, cgu::BaseGLWindow* sender) override;
        virtual bool HandleMouseApp(unsigned int buttonAction, float mouseWheelDelta, cgu::BaseGLWindow* sender) override;
        virtual void OnResize(unsigned int width, unsigned int height) override;

    private:
        /** Holds the screen text to render fps. */
        std::unique_ptr<cgu::ScreenText> fpsText;

    protected:
        void FrameMove(float time, float elapsed) override;
        void RenderScene() override;
    };
}

#endif /* FWAPPLICATION_H */
