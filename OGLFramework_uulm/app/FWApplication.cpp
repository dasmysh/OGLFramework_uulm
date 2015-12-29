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
    FWApplication::FWApplication(cgu::GLWindow& window) :
        ApplicationBase(window, glm::vec3(0.0f, 0.0f, 10.0f)),
        fpsText(new cgu::ScreenText(*fontManager->GetResource("Arial"), programManager->GetResource(fontProgramID),
            "test", glm::vec2(static_cast<float>(window.GetWidth()) - 100.0f, 10.0f), 30.0f))
    {
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

    void FWApplication::FrameMove(float time, float elapsed)
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

        cameraView->UpdateCamera();
    }

    void FWApplication::RenderScene()
    {
        win.BatchDraw([&](cgu::GLBatchRenderTarget & rt) {
            glDepthMask(GL_TRUE);
            glEnable(GL_DEPTH_TEST);
            glCullFace(GL_BACK);
            glEnable(GL_CULL_FACE);
            float clearColor[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
            rt.Clear(static_cast<unsigned int>(cgu::ClearFlags::CF_RenderTarget) | static_cast<unsigned int>(cgu::ClearFlags::CF_Depth), clearColor, 1.0, 0);
        });

        win.BatchDraw([&](cgu::GLBatchRenderTarget & rt) {
            orthoView->SetView();
            // depth of for text / GUI
            glDepthMask(GL_FALSE);
            glDisable(GL_DEPTH_TEST);
            fpsText->Draw();
        });
    }

    bool FWApplication::HandleKeyboard(unsigned int vkCode, bool bKeyDown, cgu::BaseGLWindow* sender)
    {
        if (ApplicationBase::HandleKeyboard(vkCode, bKeyDown, sender)) return true;
        if (!IsRunning() || IsPaused()) return false;
        auto handled = false;
        return handled;
    }

    bool FWApplication::HandleMouseApp(unsigned int buttonAction, float mouseWheelDelta, cgu::BaseGLWindow* sender)
    {
        return false;
    }

    void FWApplication::OnResize(unsigned int width, unsigned int height)
    {
        ApplicationBase::OnResize(width, height);
        auto fWidth = static_cast<float> (width);
        // auto fHeight = static_cast<float> (height);
        fpsText->SetPosition(glm::vec2(fWidth - 100.0f, 10.0f));
        // auto aspectRatio = fWidth / fHeight;
    }
}
