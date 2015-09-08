/**
 * @file   ApplicationBase.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   17. Dezember 2013
 *
 * @brief  Declares the application base class.
 */

#ifndef APPLICATIONBASE_H
#define APPLICATIONBASE_H

#include "core/TextureManager.h"
#include "gfx/glrenderer/ShaderBufferBindingPoints.h"
#include "core/MaterialLibManager.h"
#include "core/ShaderManager.h"
#include "core/GPUProgramManager.h"
#include "core/FontManager.h"
#include "gfx/OrthogonalView.h"
#include "gfx/CameraView.h"
#include "main.h"
#include "core/VolumeManager.h"

namespace cgu {

    class GLWindow;
    class Configuration;
    class BaseGLWindow;

    /**
     * @brief Application base.
     * <para>    Base class for all applications using this framework. </para>
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   17. Dezember 2013
     */
    class ApplicationBase
    {
    private:
        ApplicationBase(const ApplicationBase& orig) : win(orig.win) {};
        ApplicationBase& operator=(const ApplicationBase&) { return *this; };

    public:
        ApplicationBase(GLWindow& window);
        virtual ~ApplicationBase();

        /** Starts the application. */
        void StartRun();
        /** Checks if the application is still running. */
        bool IsRunning();
        /** Make one application <em>step</em> (rendering etc.). */
        void Step();
        /** Called if the application is to end running. */
        void EndRun();

        bool IsPaused() const { return m_pause; };

        virtual bool HandleKeyboard(unsigned int vkCode, bool bKeyDown, BaseGLWindow* sender);
        bool HandleMouse(unsigned int buttonAction, float mouseWheelDelta, BaseGLWindow* sender);
        virtual bool HandleMouseApp(unsigned int buttonAction, float mouseWheelDelta, BaseGLWindow* sender) = 0;
        virtual void OnResize(unsigned int width, unsigned int height);

        TextureManager* GetTextureManager();
        VolumeManager* GetVolumeManager();
        MaterialLibManager* GetMaterialLibManager();
        ShaderManager* GetShaderManager();
        GPUProgramManager* GetGPUProgramManager();
        FontManager* GetFontManager();
        ShaderBufferBindingPoints* GetUBOBindingPoints();
        ShaderBufferBindingPoints* GetSSBOBindingPoints();
        Configuration& GetConfig();
        GLWindow* GetWindow();
        GPUProgram* GetFontProgram();
        GPUProgram* GetGUIProgram();
        BindingLocation* GetGUITexUniform() { return &guiTexUniform; };

    private:
        // application status
        /// <summary>   true if application is paused. </summary>
        bool m_pause;
        /// @brief  <c>true</c> true if the application has stopped (i.e. the last scene has finished).
        bool m_stopped;

        /// <summary>   The (global) time of the application. </summary>
        double m_time;
        /// <summary>   Time elapsed in the frame. </summary>
        double m_elapsedTime;
        /// <summary>   The ticks per second of the query performance counter (QPF). </summary>
        long long m_QPFTicksPerSec;
        /// <summary>   The (QPF) time elapsed in the last frame.. </summary>
        long long m_lastElapsedTime;
        /// <summary>   The (QPF) time the application started. </summary>
        long long m_baseTime;
        /// @brief  The current scene.
        unsigned int m_currentScene;

    protected:
        /**
         * Moves a single frame.
         * @param time the time elapsed since the application started
         * @param elapsed the time elapsed since the last frame
         */
        virtual void FrameMove(float time, float elapsed) = 0;
        /** Renders the scene. */
        virtual void RenderScene() = 0;

        /** Holds the applications main window. */
        GLWindow& win;
        /** Holds the texture manager. */
        std::unique_ptr<TextureManager> texManager;
        /** Holds the volume manager. */
        std::unique_ptr<VolumeManager> volManager;
        /** Holds the material lib manager. */
        std::unique_ptr<MaterialLibManager> matManager;
        /** Holds the shader manager. */
        std::unique_ptr<ShaderManager> shaderManager;
        /** Holds the gpu program manager. */
        std::unique_ptr<GPUProgramManager> programManager;
        /** Holds the font manager. */
        std::unique_ptr<FontManager> fontManager;

        /** Holds the uniform binding points. */
        ShaderBufferBindingPoints uniformBindingPoints;
        /** Holds the shader storage buffer object binding points. */
        ShaderBufferBindingPoints shaderStorageBindingPoints;
        /** Holds the orthographic view. */
        std::unique_ptr<OrthogonalView> orthoView;
        /** Holds the perspective camera view. */
        std::unique_ptr<CameraView> cameraView;
        /** Holds the GPUProgram for font rendering. */
        GPUProgram* fontProgram;
        /** Holds the GPUProgram for gui rendering. */
        GPUProgram* guiProgram;

    protected:
        /** holds the gui programs uniform bindings. */
        BindingLocation guiTexUniform;
    };
}
#endif /* APPLICATIONBASE_H */
