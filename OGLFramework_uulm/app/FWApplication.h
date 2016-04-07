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
        explicit FWApplication(const std::string& mainWindowTitle, cgu::Configuration& config);
        virtual ~FWApplication();

        bool HandleKeyboard(int key, int scancode, int action, int mods, cgu::GLWindow* sender) override;
        bool HandleMouseApp(int button, int action, int mods, float mouseWheelDelta, cgu::GLWindow* sender) override;
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
