/**
 * @file   SpotLight.h
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.12.17
 *
 * @brief  Declaration of a spot light.
 */

#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "main.h"
#include "gfx/CameraView.h"

namespace cgu {

    struct SpotLightParams
    {
        /** Holds the lights position. */
        glm::vec4 position;
        /** Holds the lights direction. */
        glm::vec4 direction;
        /** Holds the lights intensity (power per solid angle). */
        glm::vec4 intensity;
        /** Holds the start of the angular falloff. */
        float angFalloffStart;
        /** Holds the width of the angular falloff. */
        float angFalloffWidth;
        /** Holds the distance attenuation. */
        float distAttenuation;
        /** Holds the z value of the far plane. */
        float farZ;
    };

    class SpotLight
    {
    public:
        SpotLight(const glm::vec3&  intensity, float fov, const glm::vec3& pos, ShaderBufferBindingPoints* uniformBindingPoints);
        ~SpotLight();

        // void Resize(const glm::vec2 size); // TODO: add for shadow map. [12/17/2015 Sebastian Maisch]
        bool HandleKeyboard(unsigned int vkCode, bool bKeyDown, BaseGLWindow* sender);
        bool HandleMouse(unsigned int buttonAction, float mouseWheelDelta, BaseGLWindow* sender);
        void UpdateLight();
        void UpdateLightParameters(SpotLightParams& params) const;
        /** Returns the view matrix of the light. */
        const glm::mat4& GetViewMatrix() const { return camera.GetViewMatrix(); }
        /** Returns the lights position. */
        const glm::vec3& GetPosition() const { return camera.GetPosition(); }
        /** Returns the lights intensity (const). */
        const glm::vec3& GetIntensity() const { return intensity; }
        /** Returns the lights intensity. */
        glm::vec3& GetIntensity() { return intensity; }

    private:
        /** Holds the lights camera object. */
        CameraView camera;
        /** Holds the falloff. */
        float falloffWidth;
        /** Holds the lights intensity (power per solid angle). */
        glm::vec3 intensity;
        /** Holds the lights attenuation. */
        float attenuation;
        /** Holds the lights maximum range. */
        float farPlane;
        /** Holds the shadow map bias. */
        float bias;
        // TODO: add shadow map. [12/17/2015 Sebastian Maisch]
        // ShadowMap *shadowMap;
    };

    class SpotLightArray
    {
    public:
        SpotLightArray(const std::string& lightArrayName, ShaderBufferBindingPoints* uniformBindingPoints);
        ~SpotLightArray();

        void SetLightParameters();

        /** Returns the managed lights array (const). */
        const std::vector<SpotLight>& GetLights() const { return lights; }
        /** Returns the managed lights array. */
        std::vector<SpotLight>& GetLights() { return lights; }

    private:
        /** Holds the lights. */
        std::vector<SpotLight> lights;
        /** Holds the lights parameters. */
        std::vector<SpotLightParams> lightParams;
        /** Holds the lights uniform buffer. */
        std::unique_ptr<GLUniformBuffer> lightsUBO;
    };
}

#endif // SPOTLIGHT_H
