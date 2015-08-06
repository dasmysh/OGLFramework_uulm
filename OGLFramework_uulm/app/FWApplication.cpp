/**
 * @file   FWApplication.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   21. Januar 2014
 *
 * @brief  Contains the implementation of FWApplication.
 */

#include "FWApplication.h"
#include "gfx/OBJMesh.h"
#include "gfx/glrenderer/GLUniformBuffer.h"
#include "gfx/glrenderer/GUIRenderable.h"
#include "app/GLWindow.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace cguFrameworkApp {

    /**
     * Constructor.
     * @param window the primary window used for rendering
     */
    FWApplication::FWApplication(cgu::GLWindow& window) :
        ApplicationBase(window),
        simpleProgram(nullptr),
        fontProgram(nullptr)
    {
        triangleMesh.reset(new cgu::OBJMesh("simple.obj", this));
        triangleMesh->Load();
        triangleRenderable.reset(new cgu::MeshRenderable(*triangleMesh.get()));

        simpleProgram = programManager->GetResource("simple.vp|simple.fp");
        triangleRenderable->FillVertexAttributeBindings(*simpleProgram, triangleMeshBind);
        colorUBuffer.reset(new cgu::GLUniformBuffer("colors", sizeof(glm::vec3), uniformBindingPoints));
        glm::vec3 color(1.0f, 0.0f, 0.0f);
        colorUBuffer->UploadData(0, sizeof(glm::vec3), &color);
        simpleProgram->BindUniformBlock("colors", uniformBindingPoints);

        font.reset(new cgu::Font("Arial", this));
        font->Load();
        fontProgram = programManager->GetResource(fontProgramID);

        text.reset(new cgu::ScreenText(*font.get(), fontProgram,
            "Some Text", glm::vec2(200.0f, 600.0f), 300.0f));

        // OpenGL stuff
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    FWApplication::~FWApplication()
    {
    }

    void FWApplication::FrameMove(float /* time */, float /* elapsed */)
    {
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
            orthoView->SetView();

            // depth of for text / gui
            glDepthMask(GL_FALSE);
            glDisable(GL_DEPTH_TEST);
        });
    }

    bool FWApplication::HandleKeyboard(unsigned int vkCode, bool bKeyDown, cgu::BaseGLWindow* sender)
    {
        if (ApplicationBase::HandleKeyboard(vkCode, bKeyDown, sender)) return true;
        bool handled = false;
        glm::vec3 color;
        switch (vkCode)
        {
        case 'R':
            color = glm::vec3(1.0f, 0.0f, 0.0f);
            if (colorUBuffer) colorUBuffer->UploadData(0, sizeof(glm::vec3), &color);
            handled = true;
            break;
        case 'G':
            color = glm::vec3(0.0f, 1.0f, 0.0f);
            if (colorUBuffer) colorUBuffer->UploadData(0, sizeof(glm::vec3), &color);
            handled = true;
            break;
        case 'B':
            color = glm::vec3(0.0f, 0.0f, 1.0f);
            if (colorUBuffer) colorUBuffer->UploadData(0, sizeof(glm::vec3), &color);
            handled = true;
            break;
        }
        return handled;
    }

    bool FWApplication::HandleMouseApp(unsigned int buttonAction, float mouseWheelDelta, cgu::BaseGLWindow* sender)
    {
        return false;
    }

    void FWApplication::OnResize(unsigned int width, unsigned int height)
    {
        ApplicationBase::OnResize(width, height);
        float aspectRatio = static_cast<float> (width) / static_cast<float> (height);
    }
}
