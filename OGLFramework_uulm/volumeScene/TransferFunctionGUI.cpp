/**
 * @file   TransferFunctionGUI.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.08.24
 *
 * @brief  Definition of the transfer function GUI.
 */

#include "TransferFunctionGUI.h"
#include "app/ApplicationBase.h"
#include "gfx/glrenderer/GUIRenderable.h"
#include "gfx/glrenderer/GLTexture.h"
#include "app/BaseGLWindow.h"
#include <boost/assign.hpp>
#include "gfx/glrenderer/GLUniformBuffer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "app/GLWindow.h"

namespace cgu {

    TransferFunctionGUI::TransferFunctionGUI(const glm::vec2& boxMin, const glm::vec2& boxMax, ApplicationBase* app) :
        rectMin(boxMin), rectMax(boxMax),
        quad(nullptr),
        quadTex(nullptr),
        tfTex(nullptr),
        colorPicker(),
        selection(-1),
        draggingSelection(false),
        lastButtonAction(0),
        tfProgram(nullptr),
        orthoUBO(new GLUniformBuffer("tfOrthoProjection", sizeof(OrthoProjectionBuffer), *app->GetUBOBindingPoints())),
        tfVBO(0)
    {
        screenAlignedProg = app->GetGPUProgramManager()->GetResource("tfRenderGUI.vp|tfRenderGUI.fp");
        screenAlignedTextureUniform = screenAlignedProg->GetUniformLocation("guiTex");
        screenAlignedProg->BindUniformBlock("tfOrthoProjection", *app->GetUBOBindingPoints());

        tfProgram = app->GetGPUProgramManager()->GetResource("tfPicker.vp|tfPicker.fp");
        tfProgram->BindUniformBlock("tfOrthoProjection", *app->GetUBOBindingPoints());

        std::vector<GUIVertex> quadVerts(4);
        quadVerts[0].pos = glm::vec3(0.0f);
        quadVerts[0].texCoords = glm::vec2(0.0f, 1.0f);
        quadVerts[1].pos = glm::vec3(0.0f, 1.0f, 0.0f);
        quadVerts[1].texCoords = glm::vec2(0.0f, 0.0f);
        quadVerts[2].pos = glm::vec3(1.0f, 0.0f, 0.0f);
        quadVerts[2].texCoords = glm::vec2(1.0f, 1.0f);
        quadVerts[3].pos = glm::vec3(1.0f, 1.0f, 0.0f);
        quadVerts[3].texCoords = glm::vec2(1.0f, 0.0f);
        std::vector<unsigned int> quadIndices{ 0, 1, 2, 2, 1, 3 };

        quad.reset(new GUIRenderable(quadVerts, quadIndices, screenAlignedProg));

        std::stringstream options;
        glm::uvec2 atbSize{ static_cast<unsigned int>(rectMax.x - rectMin.x), 115 };
        glm::uvec2 atbPos{ static_cast<unsigned int>(rectMin.x), static_cast<unsigned int>(rectMax.y + 10.0f) };
        if (rectMin.y >= static_cast<float>(atbSize.y + 20)) atbPos.y = static_cast<unsigned int>(rectMin.y) - (atbSize.y + 10);
        options << " PickColor position='" << atbPos.x << " " << atbPos.y << "' " << "size='" << atbSize.x << " " << atbSize.y << "' ";

        colorPicker = TwNewBar("PickColor");
        TwAddVarCB(colorPicker, "TransferPointColor", TW_TYPE_COLOR3F, TransferFunctionGUI::SetColorCallback,
            TransferFunctionGUI::GetColorCallback, this, " label='Point Color' ");
        TwDefine(" PickColor visible=false ");
        TwDefine(options.str().c_str());

        // Create default control points for TF
        tf::ControlPoint p0, p1;
        p0.SetColor(glm::vec3(0));
        p0.SetPos(glm::vec2(0));
        p1.SetColor(glm::vec3(1));
        p1.SetPos(glm::vec2(1));

        tf_.InsertControlPoint(p0);
        tf_.InsertControlPoint(p1);

        // Create texture and update it
        tfTex.reset(new GLTexture(TEX_RES, TextureDescriptor(32, GL_RGBA8, GL_RGBA, GL_FLOAT)));
        UpdateTexture();

        // Create BG texture
        std::vector<glm::vec4> texContent(TEX_RES * (TEX_RES / 2), glm::vec4(0.2f));
        quadTex.reset(new GLTexture(TEX_RES, TEX_RES / 2, TextureDescriptor(32, GL_RGBA8, GL_RGBA, GL_FLOAT), texContent.data()));

        UpdateTF(true);
        Resize(static_cast<float>(app->GetWindow()->GetWidth()), static_cast<float>(app->GetWindow()->GetHeight()));
    }


    TransferFunctionGUI::~TransferFunctionGUI()
    {
    }



    void TransferFunctionGUI::Resize(float width, float height)
    {
        float left = -rectMin.x / (rectMax.x - rectMin.x);
        float right = (width - rectMin.x) / (rectMax.x - rectMin.x);
        float bottom = (height - rectMin.y) / (rectMax.y - rectMin.y);
        float top = -rectMin.y / (rectMax.y - rectMin.y);
        orthoBuffer.orthoMatrix = glm::ortho(left, right, bottom, top, 1.0f, -1.0f);
        orthoUBO->UploadData(0, sizeof(OrthoProjectionBuffer), &orthoBuffer);
    }

    void TransferFunctionGUI::Draw()
    {
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        orthoUBO->BindBuffer();
        screenAlignedProg->UseProgram();
        quadTex->ActivateTexture(GL_TEXTURE0);
        screenAlignedProg->SetUniform(screenAlignedTextureUniform, 0);
        quad->RenderGeometry();

        // draw
        glPointSize(0.5f * pickRadius);
        tfProgram->UseProgram();
        attribBind->EnableVertexAttributeArray();
        // draw function
        glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>((tf_.points().size() + 2)));
        // draw points
        glDrawArrays(GL_POINTS, 1, static_cast<GLsizei>(tf_.points().size()));
        // draw selection
        if (selection != -1) {
            glPointSize(0.8f * pickRadius);
            glDrawArrays(GL_POINTS, selection + 1, static_cast<GLsizei>(1));
        }
        attribBind->DisableVertexAttributeArray();

        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
    }

    bool TransferFunctionGUI::HandleMouse(unsigned int buttonAction, float mouseWheelDelta, BaseGLWindow* sender)
    {
        bool handled = false;

        if (selection == -1) {
            draggingSelection = false;
            TwDefine(" PickColor visible=false ");
        }
        // if (buttonAction == 0 && !draggingSelection) return handled;

        glm::vec2 screenSize(static_cast<float>(sender->GetWidth()), static_cast<float>(sender->GetHeight()));
        glm::vec2 pickSize = glm::vec2(pickRadius) / screenSize;
        glm::vec2 mouseCoords = sender->GetMouseAbsolute();

        // calculate relative points
        glm::vec2 relCoords = (mouseCoords - rectMin) / (rectMax - rectMin);
        relCoords.y = 1.0f - relCoords.y;

        if (draggingSelection) {
            relCoords = glm::clamp(relCoords, glm::vec2(0.0f), glm::vec2(1.0f));
            // update selected point position
            selection = tf_.SetPosition(selection, relCoords);
            UpdateTF();

            // check for drop
            if (buttonAction & RI_MOUSE_LEFT_BUTTON_UP) draggingSelection = false;
            return true;
        }

        const float SEL_RAD2 = (pickRadius / (rectMax.x - rectMin.x)) * (pickRadius / (rectMax.y - rectMin.y));
        if (relCoords.x >= -SEL_RAD2 && relCoords.y >= -SEL_RAD2 && relCoords.x <= 1.0f + SEL_RAD2 && relCoords.y <= 1.0f + SEL_RAD2)
        {
            // left click: select point
            if (buttonAction & RI_MOUSE_LEFT_BUTTON_DOWN && SelectPoint(relCoords, pickSize)) {
                draggingSelection = true;
            }

            // right click: delete old point
            if (buttonAction & RI_MOUSE_RIGHT_BUTTON_DOWN && RemovePoint(relCoords, pickSize)) {
                UpdateTF();
            }

            // middle click: new point
            if (buttonAction & RI_MOUSE_MIDDLE_BUTTON_DOWN && AddPoint(relCoords, pickSize)) {
                UpdateTF();
            }

            handled = true;
        }

        return handled;
    }

    bool TransferFunctionGUI::SelectPoint(const glm::vec2& position, const glm::vec2& pickSize)
    {
        selection = GetControlPoint(position);
        if (selection != -1) {
            TwDefine(" PickColor visible=true ");
            return true;
        }
        return false;
    }

    bool TransferFunctionGUI::AddPoint(const glm::vec2& position, const glm::vec2& pickSize)
    {
        int i = GetControlPoint(position);
        selection = i;

        if (i == -1 && Overlap(position)) {
            tf::ControlPoint p;
            p.SetPos(position);
            p.SetColor(glm::vec3(tf_.RGBA(p.val)));
            tf_.InsertControlPoint(p);
            selection = GetControlPoint(position);
            TwDefine(" PickColor visible=true ");
            draggingSelection = false;
            return true;
        }

        draggingSelection = true;
        return false;
    }

    bool TransferFunctionGUI::RemovePoint(const glm::vec2& position, const glm::vec2& pickSize)
    {
        int i = GetControlPoint(position);
        if (i > -1) {
            tf_.RemoveControlPoint(i);
            selection = -1;
            return true;
        }
        return false;
    }

    void TW_CALL TransferFunctionGUI::SetColorCallback(const void *value, void *clientData)
    {
        static_cast<TransferFunctionGUI *>(clientData)->SetSelectionColor(static_cast<const glm::vec3*>(value));

    }

    void TW_CALL TransferFunctionGUI::GetColorCallback(void *value, void *clientData)
    {
        *static_cast<glm::vec3*>(value) = static_cast<const TransferFunctionGUI *>(clientData)->GetSelectionColor();
    }

    void TransferFunctionGUI::SetSelectionColor(const glm::vec3* color)
    {
        if (selection != -1) tf_.points()[selection].SetColor(*color);
        UpdateTF();
    }

    glm::vec3 TransferFunctionGUI::GetSelectionColor() const
    {
        if (selection != -1) return tf_.points()[selection].GetColor();
        return glm::vec3(0.0f);
    }

    void TransferFunctionGUI::UpdateTF(bool createVAO)
    {
        if (createVAO) {
            if (tfVBO != 0) glDeleteBuffers(1, &tfVBO);
            OGL_CALL(glGenBuffers, 1, &tfVBO);
        }

        OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, tfVBO);
        OGL_CALL(glBufferData, GL_ARRAY_BUFFER, (tf_.points().size() + 2) * sizeof(tf::ControlPoint),
            nullptr, GL_DYNAMIC_DRAW);

        tf::ControlPoint first, last;
        first = tf_.points()[0];
        first.SetValue(0.0f);
        last = tf_.points().back();
        last.SetValue(1.0f);

        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(tf::ControlPoint), &first);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(tf::ControlPoint), tf_.points().size() * sizeof(tf::ControlPoint), tf_.points().data());
        glBufferSubData(GL_ARRAY_BUFFER, (tf_.points().size() + 1) * sizeof(tf::ControlPoint), sizeof(tf::ControlPoint), &last);

        if (createVAO) {
            std::vector<BindingLocation> loc = tfProgram->GetAttributeLocations(boost::assign::list_of<std::string>("value")("color"));
            attribBind = tfProgram->CreateVertexAttributeArray(tfVBO, 0);
            attribBind->StartAttributeSetup();
            attribBind->AddVertexAttribute(loc[0], 1, GL_FLOAT, GL_FALSE, sizeof(tf::ControlPoint), 0);
            attribBind->AddVertexAttribute(loc[1], 4, GL_FLOAT, GL_FALSE, sizeof(tf::ControlPoint), sizeof(float));
            attribBind->EndAttributeSetup();
        } else {
            attribBind->UpdateVertexAttributes();
        }
        UpdateTexture();
    }

    void TransferFunctionGUI::UpdateTexture()
    {
        std::array<glm::vec4, TEX_RES> texData;
        tf_.CreateTextureData(texData.data(), TEX_RES);
        tfTex->SetData(texData.data());
    }

    // Gets an index to a control point if found within radii of mouse_pos
    int TransferFunctionGUI::GetControlPoint(const glm::vec2& mouse_pos)
    {
        const float SEL_RAD2 = (pickRadius / (rectMax.x - rectMin.x)) * (pickRadius / (rectMax.y - rectMin.y));

        for (int i = 0; i < (int)tf_.points().size(); ++i) {
            glm::vec2 p = tf_.points()[i].GetPos();
            // p.y = 1.f - p.y;
            // glm::vec2 pos = p * (rectMax - rectMin) + rectMin;
            glm::vec2 d = mouse_pos - p;
            float dist2 = d.x * d.x + d.y * d.y;
            if (dist2 < SEL_RAD2)
                return i;
        }
        return -1;
    }
}
