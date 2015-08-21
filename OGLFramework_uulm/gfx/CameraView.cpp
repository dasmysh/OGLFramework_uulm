/**
 * @file   CameraView.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.05.27
 *
 * @brief  Implementation of the free fly camera.
 */

#define GLM_SWIZZLE

#include "CameraView.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/quaternion.hpp>
#include "glrenderer/GLUniformBuffer.h"

namespace cgu {

    CameraView::CameraView(float theFovY, float theAspectRatio, float theNearZ, float theFarZ, glm::vec3 theCamPos, ShaderBufferBindingPoints* uniformBindingPoints) :
        fovY(theFovY),
        aspectRatio(theAspectRatio),
        nearZ(theNearZ),
        farZ(theFarZ),
        camPos(theCamPos),
        camOrient(1.0f, 0.0f, 0.0f, 0.0f),
        camUp(0.0f, 1.0f, 0.0f),
        camArcball(RI_MOUSE_LEFT_BUTTON_DOWN, MB_LEFT),
        perspectiveUBO(new GLUniformBuffer(perspectiveProjectionUBBName, sizeof(PerspectiveTransformBuffer), *uniformBindingPoints))
    {
        Resize(aspectRatio);
    }

    CameraView::~CameraView()
    {
    }

    void CameraView::Resize(float theAspectRatio)
    {
        aspectRatio = theAspectRatio;
        perspective = glm::perspective(glm::radians(fovY), aspectRatio, nearZ, farZ);
        view = glm::lookAt(camPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    void CameraView::UpdateCamera()
    {
        camOrient = camArcball.GetWorldRotation(glm::inverse(view)) * camOrient;
        glm::mat3 camOrientMat = glm::mat3_cast(camOrient);
        glm::vec3 target(0.0f);
        glm::vec3 camOffset = camOrientMat * camPos;
        glm::vec3 pos = camOffset - target; // -camOffset;
        glm::vec3 up = camOrientMat * glm::vec3(0.0f, 1.0f, 0.0f);
        // camPos = glm::mat3_cast(camArcball.GetWorldRotation(view)) * camPos;
        view = glm::lookAt(pos, target, up);
    }

    void CameraView::SetView(const glm::mat4& modelM) const
    {
        PerspectiveTransformBuffer perspectiveBuffer;
        perspectiveBuffer.mat_m = modelM;
        perspectiveBuffer.mat_mvp = perspective * view * modelM;
        perspectiveBuffer.mat_normal = glm::mat4(glm::mat3(modelM));

        perspectiveUBO->UploadData(0, sizeof(PerspectiveTransformBuffer), &perspectiveBuffer);
        perspectiveUBO->BindBuffer();
    }

    bool CameraView::HandleKeyboard(unsigned int vkCode, bool bKeyDown, BaseGLWindow* sender)
    {
        bool handled = false;
        switch (vkCode) {
        case 'W':
            camPos -= glm::vec3(0.0f, 0.0f, 0.5f);
            handled = true;
            break;
        case 'A':
            camPos -= glm::vec3(0.5f, 0.0f, 0.0f);
            handled = true;
            break;
        case 'S':
            camPos += glm::vec3(0.0f, 0.0f, 0.5f);
            handled = true;
            break;
        case 'D':
            camPos += glm::vec3(0.5f, 0.0f, 0.0f);
            handled = true;
            break;
        case VK_SPACE:
            camPos += glm::vec3(0.0f, 0.5f, 0.0f);
            handled = true;
            break;
        case VK_CONTROL:
        case VK_LCONTROL:
            camPos -= glm::vec3(0.0f, 0.5f, 0.0f);
            handled = true;
            break;
        }

        view = glm::lookAt(camPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        return handled;
    }

    bool CameraView::HandleMouse(unsigned int buttonAction, float mouseWheelDelta, BaseGLWindow* sender)
    {
        bool handled = camArcball.HandleMouse(buttonAction, sender);

        if (mouseWheelDelta != 0) {
            fovY -= mouseWheelDelta * 0.03f;
            fovY = glm::clamp(fovY, 1.0f, 80.0f);
            perspective = glm::perspective(glm::radians(fovY), aspectRatio, nearZ, farZ);
            handled = true;
        }

        return handled;
    }
}
