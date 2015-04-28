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
#include "main.h"
// TODO: no gui today
// #include "gui/GUIThemeManager.h"

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

    virtual void HandleKeyDown(unsigned int keyCode);
    virtual void HandleKeyboardComplete(unsigned int ch, bool bKeyDown, bool bAltDown);
    virtual void HandleMouse(bool bLeftButtonDown, bool bRightButtonDown,
            bool bMiddleButtonDown, int nMouseWheelDelta, int xPos, int yPos);
    virtual void OnResize(unsigned int width, unsigned int height);

    TextureManager* GetTextureManager();
    MaterialLibManager* GetMaterialLibManager();
    ShaderManager* GetShaderManager();
    GPUProgramManager* GetGPUProgramManager();
    FontManager* GetFontManager();
    // GUIThemeManager* GetGUIThemeManager();
    ShaderBufferBindingPoints* GetUBOBindingPoints();
    Configuration& GetConfig();
    GLWindow* GetWindow();
    GPUProgram* GetFontProgram();
    GPUProgram* GetGUIProgram();

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
    /** Holds the material lib manager. */
    std::unique_ptr<MaterialLibManager> matManager;
    /** Holds the shader manager. */
    std::unique_ptr<ShaderManager> shaderManager;
    /** Holds the gpu program manager. */
    std::unique_ptr<GPUProgramManager> programManager;
    /** Holds the font manager. */
    std::unique_ptr<FontManager> fontManager;
    /** Holds the gpu program manager. */
    // std::unique_ptr<GUIThemeManager> guiThemeManager = {};
    /** Holds the uniform binding points. */
    ShaderBufferBindingPoints uniformBindingPoints;
    /** Holds the orthographic view. */
    std::unique_ptr<OrthogonalView> orthoView;
    /** Holds the GPUProgram for font rendering. */
    GPUProgram* fontProgram;
    /** Holds the GPUProgram for gui rendering. */
    GPUProgram* guiProgram;

protected:
    /** holds the gui programs uniform bindings. */
    BindingLocation guiTexUniform;
};

#endif /* APPLICATIONBASE_H */
