/**
 * @file   FWApplication.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.01.21
 *
 * @brief  Contains the implementation of FWApplication.
 */

#include "FWApplication.h"
#include "app/GLWindow.h"

#include <glm/glm.hpp>

namespace cguFrameworkApp {

    /**
     * Constructor.
     * @param window the primary window used for rendering
     */
    FWApplication::FWApplication(const std::string& mainWindowTitle, cgu::Configuration& config) :
        ApplicationBase(mainWindowTitle, config, glm::vec3(0.0f, 0.0f, 10.0f)),
        fpsText(new cgu::ScreenText(GetFontManager()->GetResource("Arial"), GetGPUProgramManager()->GetResource(fontProgramID),
            "test", glm::vec2(static_cast<float>(GetWindow()->GetWidth()) - 100.0f, 10.0f), 30.0f))
    {
        int status = gladLoadGL();
        // OpenGL stuff
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glFrontFace(GL_CCW);
    }

    FWApplication::~FWApplication()
    {
    }

    void FWApplication::FrameMove(float, float elapsed)
    {
        static auto fps = 0.0f;
        static auto accumulatedElapsed = 0.0f;
        static auto numAvg = 0.0f;

        accumulatedElapsed += elapsed;
        numAvg += 1.0f;
        if (accumulatedElapsed > 0.5f) {
            fps = numAvg / accumulatedElapsed;
            accumulatedElapsed = 0.0f;
            numAvg = 0.0f;
        }
        std::stringstream fpsString;
        fpsString << fps;
        fpsText->SetText(fpsString.str());

        GetCameraView()->UpdateCamera();
    }

    void FWApplication::RenderScene()
    {
        GetWindow()->BatchDraw([&](cgu::GLBatchRenderTarget & rt) {
            glDepthMask(GL_TRUE);
            glEnable(GL_DEPTH_TEST);
            glCullFace(GL_BACK);
            glEnable(GL_CULL_FACE);
            float clearColor[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
            rt.Clear(static_cast<unsigned int>(cgu::ClearFlags::CF_RenderTarget) | static_cast<unsigned int>(cgu::ClearFlags::CF_Depth), clearColor, 1.0, 0);
        });

        GetWindow()->BatchDraw([&](cgu::GLBatchRenderTarget & rt) {
            GetOrthoginalView()->SetView();
            // depth of for text / GUI
            glDepthMask(GL_FALSE);
            glDisable(GL_DEPTH_TEST);
            fpsText->Draw();
        });
    }

    void FWApplication::RenderGUI()
    {
    }

    bool FWApplication::HandleKeyboard(int key, int scancode, int action, int mods, cgu::GLWindow* sender)
    {
        if (ApplicationBase::HandleKeyboard(key, scancode, action, mods, sender)) return true;
        if (!IsRunning() || IsPaused()) return false;
        auto handled = false;
        return handled;
    }

    bool FWApplication::HandleMouseApp(int, int, int, float, cgu::GLWindow*)
    {
        return false;
    }

    void FWApplication::Resize(const glm::uvec2& screenSize)
    {
        fpsText->SetPosition(glm::vec2(static_cast<float>(screenSize.x) - 100.0f, 10.0f));
    }
}
