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
#include "app/GLWindow.h"

#include <anttweakbar/AntTweakBar.h>

namespace cgu {
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
        shaderStorageBindingPoints(),
        orthoView(),
        cameraView(),
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
        cameraView.reset(new CameraView(60.0f, aspectRatio, 1.0f, 100.0f, glm::vec3(5.0f, 3.0f, 10.0f), &uniformBindingPoints));

        TwInit(TW_OPENGL_CORE, NULL);
        fontProgram = programManager->GetResource(fontProgramID);
        fontProgram->BindUniformBlock(orthoProjectionUBBName, uniformBindingPoints);
        guiProgram = programManager->GetResource(guiProgramID);
        guiTexUniform = guiProgram->GetUniformLocation("guiTex");
        guiProgram->BindUniformBlock(orthoProjectionUBBName, uniformBindingPoints);
    }

    ApplicationBase::~ApplicationBase()
    {
        TwTerminate();
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
    * Returns the shader storage buffer object binding points
    * @return the ssbo binding points
    */
    ShaderBufferBindingPoints* ApplicationBase::GetSSBOBindingPoints()
    {
        return &shaderStorageBindingPoints;
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
     * Handles all keyboard input.
     * @param vkCode the key pressed
     * @param bKeyDown <code>true</code> if the key is pressed, else it is released
     * @param sender the window that send the keyboard messages
     */
    bool ApplicationBase::HandleKeyboard(unsigned int vkCode, bool bKeyDown, BaseGLWindow* sender)
    {
        int handled = 0;
        static unsigned __int64 s_PrevKeyDown = 0;
        static __int64 s_PrevKeyDownMod = 0;
        static int s_PrevKeyDownHandled = 0;

        int kmod = 0;
        int testkp = 0;
        int k = 0;
        unsigned int twVKCode = vkCode == VK_NUMRETURN ? VK_RETURN : vkCode;
        if (sender->GetKeyboardModState(VK_MOD_SHIFT)) kmod |= TW_KMOD_SHIFT;
        if (sender->GetKeyboardModState(VK_MOD_CTRL)) {
            kmod |= TW_KMOD_CTRL; testkp = 1;
        }
        if (sender->GetKeyboardModState(VK_MOD_MENU)) {
            kmod |= TW_KMOD_ALT;  testkp = 1;
        }

        if (twVKCode >= VK_F1 && twVKCode <= VK_F15)
            k = TW_KEY_F1 + (twVKCode - VK_F1);
        else if (testkp && twVKCode >= VK_NUMPAD0 && twVKCode <= VK_NUMPAD9)
            k = '0' + (twVKCode - VK_NUMPAD0);
        else switch (twVKCode)
        {
        case VK_UP:
            k = TW_KEY_UP;
            break;
        case VK_DOWN:
            k = TW_KEY_DOWN;
            break;
        case VK_LEFT:
            k = TW_KEY_LEFT;
            break;
        case VK_RIGHT:
            k = TW_KEY_RIGHT;
            break;
        case VK_INSERT:
            k = TW_KEY_INSERT;
            break;
        case VK_DELETE:
            k = TW_KEY_DELETE;
            break;
        case VK_PRIOR:
            k = TW_KEY_PAGE_UP;
            break;
        case VK_NEXT:
            k = TW_KEY_PAGE_DOWN;
            break;
        case VK_HOME:
            k = TW_KEY_HOME;
            break;
        case VK_END:
            k = TW_KEY_END;
            break;
        case VK_DIVIDE:
            if (testkp)
                k = '/';
            break;
        case VK_MULTIPLY:
            if (testkp)
                k = '*';
            break;
        case VK_SUBTRACT:
            if (testkp)
                k = '-';
            break;
        case VK_ADD:
            if (testkp)
                k = '+';
            break;
        case VK_DECIMAL:
            if (testkp)
                k = '.';
            break;
        default:
            if ((kmod&TW_KMOD_CTRL) && (kmod&TW_KMOD_ALT))
                k = MapVirtualKey(twVKCode, 2) & 0x0000FFFF;
        }

        if (bKeyDown) {
            if (k != 0)
                handled = TwKeyPressed(k, kmod);
            else
            {
                // if the key will be handled at next WM_CHAR report this event as handled
                int key = (int)(MapVirtualKey(twVKCode, 2) & 0xff);
                if (kmod&TW_KMOD_CTRL && key > 0 && key < 27)
                    key += 'a' - 1;
                if (key > 0 && key < 256)
                    handled = TwKeyPressed(key, kmod);
            }
            s_PrevKeyDown = twVKCode;
            s_PrevKeyDownMod = kmod;
            s_PrevKeyDownHandled = handled;
        } else {
            // if the key has been handled at previous WM_KEYDOWN report this event as handled
            if (s_PrevKeyDown == twVKCode && s_PrevKeyDownMod == kmod)
                handled = s_PrevKeyDownHandled;
            else
            {
                // if the key would have been handled report this event as handled
                int key = (int)(MapVirtualKey(twVKCode, 2) & 0xff);
                if (kmod&TW_KMOD_CTRL && key > 0 && key < 27)
                    key += 'a' - 1;
                if (key > 0 && key < 256)
                    handled = TwKeyTest(key, kmod);
            }
            // reset previous keydown
            s_PrevKeyDown = 0;
            s_PrevKeyDownMod = 0;
            s_PrevKeyDownHandled = 0;
        }

        if (bKeyDown && handled == 0) {
            switch (vkCode)
            {
            case VK_ESCAPE:
                this->win.CloseWindow();
                handled = 1;
                break;
            case VK_F9:
                this->programManager->RecompileAll();
                handled = 1;
                break;
            }
        }

        if (handled == 0) handled = cameraView->HandleKeyboard(vkCode, bKeyDown, sender);

        return handled == 1;
    }

    /**
     * Handles mouse input.
     * @param buttonAction mouse action flags, see RAWMOUSE.usButtonFlags
     * @param mouseWheelDelta the scroll wheel delta
     * @param sender the window that send the keyboard messages
     * @return whether the message was handled
     */
    bool ApplicationBase::HandleMouse(unsigned int buttonAction, float mouseWheelDelta, BaseGLWindow* sender)
    {
        int handled = 0;
        int handledMovement = 0;
        if (sender->HadPositionUpdate()) {
            handledMovement = TwMouseMotion(static_cast<int>(sender->GetMouseAbsolute().x), static_cast<int>(sender->GetMouseAbsolute().y));
            sender->HandledPositionUpdate();
        }
        if (handled == 0 && buttonAction & RI_MOUSE_LEFT_BUTTON_DOWN) handled = TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_LEFT);
        if (handled == 0 && buttonAction & RI_MOUSE_LEFT_BUTTON_UP) handled = TwMouseButton(TW_MOUSE_RELEASED, TW_MOUSE_LEFT);
        if (handled == 0 && buttonAction & RI_MOUSE_RIGHT_BUTTON_DOWN) handled = TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_RIGHT);
        if (handled == 0 && buttonAction & RI_MOUSE_RIGHT_BUTTON_UP) handled = TwMouseButton(TW_MOUSE_RELEASED, TW_MOUSE_RIGHT);
        if (handled == 0 && buttonAction & RI_MOUSE_MIDDLE_BUTTON_DOWN) handled = TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_MIDDLE);
        if (handled == 0 && buttonAction & RI_MOUSE_MIDDLE_BUTTON_UP) handled = TwMouseButton(TW_MOUSE_RELEASED, TW_MOUSE_MIDDLE);

        static int s_WheelPos = 0;
        s_WheelPos += static_cast<int>(mouseWheelDelta);
        if (handled == 0) handled = TwMouseWheel(s_WheelPos);
        if (handled == 0) handled = HandleMouseApp(buttonAction, mouseWheelDelta, sender);
        // if (handledM)
        if (handled == 0) handled = cameraView->HandleMouse(buttonAction, mouseWheelDelta, sender);
        return handled == 1;
    }

    /**
     * Handles resize events.
     */
    void ApplicationBase::OnResize(unsigned int width, unsigned int height)
    {
        float aspectRatio = static_cast<float> (width) / static_cast<float> (height);
        TwWindowSize(width, height);
        if (orthoView) {
            orthoView->Resize(aspectRatio);
            orthoView->SetView();
        }
        if (cameraView) {
            cameraView->Resize(aspectRatio);
        }
    }

    void ApplicationBase::StartRun()
    {
        GLBatchRenderTarget& brt = win;
        brt.EnableAlphaBlending();
        this->m_stopped = false;
        this->m_pause = false;
        LARGE_INTEGER qwTicksPerSec = { 0, 0 };
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

        this->m_elapsedTime = (float)((double)(qwTime.QuadPart - this->m_lastElapsedTime) / (double) this->m_QPFTicksPerSec);
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
        TwDraw();
        this->win.Present();

        //this->m_dialog.Render(this->m_time, (float)this->m_elapsedTime);
        //    LOG(Log::LogLevel::LL_DEBUG) << L"-----  End of frame -" << this->m_frameCount++ << "-.  -----";
    }
}
