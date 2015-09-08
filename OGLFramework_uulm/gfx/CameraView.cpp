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
        view = glm::lookAt(camPos, glm::vec3(0.0f), camUp);
    }

    void CameraView::UpdateCamera()
    {
        glm::quat camOrientStep = camArcball.GetWorldRotation(view);
        glm::mat3 matOrientStep{ glm::mat3_cast(camOrientStep) };
        camOrient = camOrientStep * camOrient;
        glm::mat3 matOrient{ glm::mat3_cast(camOrient) };
        
        camUp = matOrient[1];
        camPos = matOrientStep * camPos;

        view = glm::lookAt(camPos, glm::vec3(0.0f), camUp);
    }

    cguMath::Frustum<float> CameraView::SetView(const glm::mat4& modelM) const
    {
        PerspectiveTransformBuffer perspectiveBuffer;
        perspectiveBuffer.mat_m = modelM;
        perspectiveBuffer.mat_mvp = perspective * view * modelM;
        perspectiveBuffer.mat_normal = glm::mat4(glm::mat3(modelM));

        perspectiveUBO->UploadData(0, sizeof(PerspectiveTransformBuffer), &perspectiveBuffer);
        perspectiveUBO->BindBuffer();

        return std::move(CalcViewFrustum(perspectiveBuffer.mat_mvp));
    }

    cguMath::Frustum<float> CameraView::GetViewFrustum(const glm::mat4& modelM) const
    {
        glm::mat4 mvp = perspective * view * modelM;
        return std::move(CalcViewFrustum(mvp));
    }

    cguMath::Frustum<float> CameraView::CalcViewFrustum(const glm::mat4& m) const
    {
        cguMath::Frustum<float> f;
        f.left() = glm::vec4{ m[0][3] + m[0][0], m[1][3] + m[1][0], m[2][3] + m[2][0], m[3][3] + m[3][0] };
        f.rght() = glm::vec4{ m[0][3] - m[0][0], m[1][3] - m[1][0], m[2][3] - m[2][0], m[3][3] - m[3][0] };
        f.bttm() = glm::vec4{ m[0][3] + m[0][1], m[1][3] + m[1][1], m[2][3] + m[2][1], m[3][3] + m[3][1] };
        f.topp() = glm::vec4{ m[0][3] - m[0][1], m[1][3] - m[1][1], m[2][3] - m[2][1], m[3][3] - m[3][1] };
        f.nrpl() = glm::vec4{ m[0][3] + m[0][2], m[1][3] + m[1][2], m[2][3] + m[2][2], m[3][3] + m[3][2] };
        f.farp() = glm::vec4{ m[0][3] - m[0][2], m[1][3] - m[1][2], m[2][3] - m[2][2], m[3][3] - m[3][2] };
        /*f.left() /= glm::length(glm::vec3(f.left()));
        f.rght() /= glm::length(glm::vec3(f.rght()));
        f.bttm() /= glm::length(glm::vec3(f.bttm()));
        f.topp() /= glm::length(glm::vec3(f.topp()));
        f.nrpl() /= glm::length(glm::vec3(f.nrpl()));
        f.farp() /= glm::length(glm::vec3(f.farp()));*/
        return std::move(f);
    }

    float CameraView::GetSignedDistanceToUnitAABB2(const glm::mat4& world) const
    {
        glm::vec3 localCamPos = glm::vec3(glm::inverse(world) * glm::vec4(camPos, 1.0f));
        glm::vec3 clampedCamPos = glm::vec3(world * glm::vec4(glm::clamp(localCamPos, glm::vec3(0.0f), glm::vec3(1.0f)), 1.0f));
        return glm::dot(clampedCamPos - camPos, clampedCamPos - camPos);
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

        view = glm::lookAt(camPos, glm::vec3(0.0f), camUp);
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
