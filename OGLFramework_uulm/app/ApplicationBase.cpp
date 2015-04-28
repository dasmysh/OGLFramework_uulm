/**
 * @file   ApplicationBase.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   17. Dezember 2013
 *
 * @brief  Implements the application base class.
 */

#include "ApplicationBase.h"
#include "gfx/glrenderer/GLUniformBuffer.h"
#include "gfx/OrthogonalView.h"
#include "core/FontManager.h"

/**
 * Construct a new application.
 * @param window the applications main window
 */
ApplicationBase::ApplicationBase(GLWindow& window) :
m_pause(true),
m_stopped(false),
m_time(0.0),
m_elapsedTime(0.0),
m_QPFTicksPerSec(0),
m_lastElapsedTime(0),
m_baseTime(0),
m_currentScene(0),
win(window),
texManager(),
matManager(),
shaderManager(),
programManager(),
fontManager(),
uniformBindingPoints(),
orthoView(),
fontProgram(nullptr),
guiProgram(nullptr),
guiTexUniform()
{
    texManager.reset(new TextureManager(this));
    matManager.reset(new MaterialLibManager(this));
    shaderManager.reset(new ShaderManager(this));
    programManager.reset(new GPUProgramManager(this));
    fontManager.reset(new FontManager(this));
    // guiThemeManager.reset(new GUIThemeManager(this));
    win.RegisterApplication(*this);
    win.ShowWindow();
    float aspectRatio = static_cast<float> (win.GetWidth())
            / static_cast<float> (win.GetHeight());
    orthoView.reset(new OrthogonalView(aspectRatio, &uniformBindingPoints));
    fontProgram = programManager->GetResource(fontProgramID);
    fontProgram->BindUniformBlock(orthoProjectionUBBName, uniformBindingPoints);
    guiProgram = programManager->GetResource(guiProgramID);
    guiTexUniform = guiProgram->GetUniformLocation("guiTex");
    guiProgram->BindUniformBlock(orthoProjectionUBBName, uniformBindingPoints);
}

ApplicationBase::~ApplicationBase()
{
}

/**
 * Returns the texture manager.
 * @return  the texture manager
 */
TextureManager* ApplicationBase::GetTextureManager()
{
    return texManager.get();
}

/**
 * Returns the material lib manager.
 * @return the material lib manager
 */
MaterialLibManager* ApplicationBase::GetMaterialLibManager()
{
    return matManager.get();
}

/**
 * Returns the shader manager.
 * @return the shader manager
 */
ShaderManager* ApplicationBase::GetShaderManager()
{
    return shaderManager.get();
}

/**
 * Returns the gpu program manager.
 * @return the gpu program manager
 */
GPUProgramManager* ApplicationBase::GetGPUProgramManager()
{
    return programManager.get();
}

/**
 * Returns the uniform buffer binding points
 * @return the ubo binding points
 */
ShaderBufferBindingPoints* ApplicationBase::GetUBOBindingPoints()
{
    return &uniformBindingPoints;
}

/**
 * Returns the current configuration.
 * @return the configuration
 */
Configuration& ApplicationBase::GetConfig()
{
    return win.GetConfig();
}

/**
 * Returns the font manager.
 * @return the font manager
 */
FontManager* ApplicationBase::GetFontManager()
{
    return fontManager.get();
}

/**
 * Returns the gui theme manager.
 * @return the gui theme manager
 */
//GUIThemeManager* ApplicationBase::GetGUIThemeManager()
//{
//    return guiThemeManager.get();
//}

/**
 * Returns the main window.
 * @return the main window
 */
GLWindow* ApplicationBase::GetWindow()
{
    return &win;
}

/**
 * Returns the gpu program for font rendering.
 * @return the font rendering program
 */
GPUProgram* ApplicationBase::GetFontProgram()
{
    return fontProgram;
}

/**
 * Returns the gpu program for gui rendering.
 * @return the gui rendering program
 */
GPUProgram* ApplicationBase::GetGUIProgram()
{
    return guiProgram;
}

/**
 * Handles a key down event. Used for fast key checks.
 * @param keyCode the key code (see PK_* key codes)
 */
void ApplicationBase::HandleKeyDown(unsigned int keyCode)
{
    switch (keyCode)
    {
        case VK_ESCAPE:
        {
            this->win.CloseWindow();
            break;
        }
        case VK_F9:
        {
            this->programManager->RecompileAll();
            break;
        }
    }
}

/**
 * Handles all keyboard input.
 * @param ch the key pressed (see PK_* key codes)
 * @param bKeyDown <code>true</code> if the key is pressed
 * @param bAltDown <code>true</code> if the alt key is pressed
 */
void ApplicationBase::HandleKeyboardComplete(unsigned int, bool, bool)
{

}

/**
 * Handles mouse input.
 * @param bLeftButtonDown <code>true</code> if the left mouse button is pressed
 * @param bRightButtonDown <code>true</code> if the right mouse button is pressed
 * @param bMiddleButtonDown <code>true</code> if the middle mouse button is pressed
 * @param nMouseWheelDelta the scroll wheel delta
 * @param xPos the mouse x position
 * @param yPos the mouse y position
 */
void ApplicationBase::HandleMouse(bool, bool, bool, int, int, int)
{

}

/**
 * Handles resize events.
 */
void ApplicationBase::OnResize(unsigned int width, unsigned int height)
{
    float aspectRatio = static_cast<float> (width) / static_cast<float> (height);
    orthoView->Resize(aspectRatio);
    orthoView->SetView();
}

void ApplicationBase::StartRun()
{
    GLBatchRenderTarget& brt = win;
    brt.EnableAlphaBlending();
    this->m_stopped = false;
    this->m_pause = false;
    LARGE_INTEGER qwTicksPerSec = {0, 0};
    QueryPerformanceFrequency(&qwTicksPerSec);
    this->m_QPFTicksPerSec = qwTicksPerSec.QuadPart;

    LARGE_INTEGER qwTime;
    QueryPerformanceCounter(&qwTime);
    this->m_baseTime = qwTime.QuadPart;
    this->m_lastElapsedTime = qwTime.QuadPart;
    orthoView->SetView();
}

bool ApplicationBase::IsRunning()
{
    return !this->m_stopped;
}

void ApplicationBase::EndRun()
{
    this->m_stopped = true;
}

void ApplicationBase::Step()
{
    if (this->m_stopped) {
        Sleep(500);
        return;
    }
    //    LOG(Log::LogLevel::LL_DEBUG) << L"-----  Begin of frame -" << this->m_frameCount << "-.  -----";
    LARGE_INTEGER qwTime;
    QueryPerformanceCounter(&qwTime);

    this->m_elapsedTime = (float) ((double) (qwTime.QuadPart - this->m_lastElapsedTime) / (double) this->m_QPFTicksPerSec);
    if (this->m_elapsedTime < 0.0f)
        this->m_elapsedTime = 0.0f;

    this->m_lastElapsedTime = qwTime.QuadPart;

    if (this->m_pause) {
        Sleep(50);
        return;
    }

    this->m_time = (qwTime.QuadPart - this->m_baseTime) / (double) this->m_QPFTicksPerSec;

    this->FrameMove((float) this->m_time, (float) this->m_elapsedTime);
    this->RenderScene();
    this->win.Present();

    //this->m_dialog.Render(this->m_time, (float)this->m_elapsedTime);
    //    LOG(Log::LogLevel::LL_DEBUG) << L"-----  End of frame -" << this->m_frameCount++ << "-.  -----";
}
