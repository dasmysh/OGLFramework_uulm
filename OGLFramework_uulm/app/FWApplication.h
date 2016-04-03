/**
 * @file   FWApplication.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.01.21
 *
 * @brief  Contains the definition of FWApplication.
 */

#ifndef FWAPPLICATION_H
#define FWAPPLICATION_H

#include "app/ApplicationBase.h"
#include "gfx/glrenderer/ScreenText.h"

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
        explicit FWApplication(cgu::GLWindow& window);
        virtual ~FWApplication();

        bool HandleKeyboard(unsigned int vkCode, bool bKeyDown, cgu::BaseGLWindow* sender) override;
        bool HandleMouseApp(unsigned int buttonAction, float mouseWheelDelta, cgu::BaseGLWindow* sender) override;
		void Resize(const glm::uvec2& screenSize) override;

    private:
        /** Holds the screen text to render fps. */
        std::unique_ptr<cgu::ScreenText> fpsText;

    protected:
        void FrameMove(float time, float elapsed) override;
        void RenderScene() override;
        void RenderGUI() override;
    };
}

#endif /* FWAPPLICATION_H */
