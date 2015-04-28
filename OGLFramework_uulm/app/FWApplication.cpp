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
// #include "gui/GroupLayout.h"

#include <glm/glm.hpp>

/**
 * Constructor.
 * @param window the primary window used for rendering
 */
FWApplication::FWApplication(GLWindow& window) :
ApplicationBase(window),
program(nullptr),
fontProgram(nullptr),
quad(nullptr),
quadTex(nullptr)
{
    objMesh.reset(new OBJMesh("simple.obj", this));
    objMesh->Load();
    mesh.reset(new MeshRenderable(*objMesh.get()));
    program = programManager->GetResource("simple.vp|simple.fp");
    mesh->FillVertexAttributeBindings(*program, bind);
    uBuffer.reset(new GLUniformBuffer("colors", sizeof (glm::vec3), uniformBindingPoints));
    glm::vec3 color(1.0f, 0.0f, 0.0f);
    uBuffer->UploadData(0, sizeof (glm::vec3), &color);
    program->BindUniformBlock("colors", uniformBindingPoints);
    font.reset(new Font("Arial", this));
    font->Load();
    fontProgram = programManager->GetResource(fontProgramID);

    text.reset(new ScreenText(*font.get(), fontProgram,
            "Some Text", glm::vec2(200.0f, 600.0f), 300.0f));
    std::vector<GUIVertex> quadVerts;
    GUIVertex v0, v1, v2, v3;
    v0.pos = glm::vec3(10.0f, 10.0f, 0.0f);
    v0.texCoords = glm::vec2(0.0f, 1.0f);
    v1.pos = glm::vec3(610.0f, 10.0f, 0.0f);
    v1.texCoords = glm::vec2(1.0f, 1.0f);
    v2.pos = glm::vec3(10.0f, 610.0f, 0.0f);
    v2.texCoords = glm::vec2(0.0f, 0.0f);
    v3.pos = glm::vec3(610.0f, 610.0f, 0.0f);
    v3.texCoords = glm::vec2(1.0f, 0.0f);
    quadVerts.push_back(v0);
    quadVerts.push_back(v2);
    quadVerts.push_back(v1);
    quadVerts.push_back(v3);
    std::vector<unsigned int> quadIndices;
    quadIndices.push_back(0);
    quadIndices.push_back(1);
    quadIndices.push_back(2);
    quadIndices.push_back(2);
    quadIndices.push_back(1);
    quadIndices.push_back(3);
    quad.reset(new GUIRenderable(quadVerts, quadIndices, this->GetGUIProgram()));
    std::vector<glm::vec4> texContent;
    texContent.resize(64*64, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    quadTex.reset(new GLTexture(64, 64, 128, texContent.data()));
}

FWApplication::~FWApplication()
{
}

void FWApplication::FrameMove(float /* time */, float /* elapsed */)
{
}

void FWApplication::RenderScene()
{
    win.BatchDraw([&](GLBatchRenderTarget & rt) {
        float clearColor[4] = {0.0f, 0.0f, 1.0f, 0.0f};
        rt.Clear(static_cast<unsigned int>(ClearFlags::CF_RenderTarget), clearColor, 1.0, 0);
        orthoView->SetView();
        // dialogManager->Render();
		program->UseProgram();
		mesh->Draw(bind);
        text->Draw();
        guiProgram->UseProgram();
        quadTex->ActivateTexture(GL_TEXTURE0);
        this->GetGUIProgram()->SetUniform(guiTexUniform, 0);
        quad->RenderGeometry();
		quadTex->ActivateTexture(GL_TEXTURE0);
        this->GetGUIProgram()->SetUniform(guiTexUniform, 0);
    });
}

void FWApplication::HandleKeyDown(unsigned int keyCode)
{
    glm::vec3 color;
    switch (keyCode)
    {
        case 'R':
            color = glm::vec3(1.0f, 0.0f, 0.0f);
            uBuffer->UploadData(0, sizeof (glm::vec3), &color);
            break;
        case 'G':
            color = glm::vec3(0.0f, 1.0f, 0.0f);
            uBuffer->UploadData(0, sizeof (glm::vec3), &color);
            break;
        case 'B':
            color = glm::vec3(0.0f, 0.0f, 1.0f);
            uBuffer->UploadData(0, sizeof (glm::vec3), &color);
            break;
    }
    ApplicationBase::HandleKeyDown(keyCode);
}
