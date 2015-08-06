/**
 * @file   WinGLWindow.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   18. Dezember 2013
 * @ingroup win
 *
 * @brief  Windows implementation for the GLWindow.
 */

#ifdef _WIN32

#include "GLWindow.h"
#include "gfx/glrenderer/FrameBuffer.h"
#include "ApplicationBase.h"
#include "gfx/glrenderer/GLUniformBuffer.h"

#include <cuda_runtime_api.h>
#include <cuda_gl_interop.h>
#include <codecvt>

namespace cgu {

    /**
     * Global function for window message handling.
     * @ingroup win
     * @param hWnd the handle of the window
     * @param message the message recieved
     * @param wParam the wparam
     * @param lParam the lparam
     * @return the handling result
     */
    LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

    /**
     * Logs the debug output of OpenGL.
     * @param source the source of the debug message
     * @param type the type of debug message
     * @param severity the severity of the debug message
     * @param message the debug message
     */
    void APIENTRY DebugOutputCallback(GLenum source, GLenum type, GLuint, GLenum severity,
        GLsizei, const GLchar* message, const void*)
    {
        std::stringstream str;
        str << "OpenGL Debug Output message : "; // netbeans console output bug

        if (source == GL_DEBUG_SOURCE_API_ARB) str << "Source : API; ";
        else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB) str << "Source : WINDOW_SYSTEM; ";
        else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB) str << "Source : SHADER_COMPILER; ";
        else if (source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB) str << "Source : THIRD_PARTY; ";
        else if (source == GL_DEBUG_SOURCE_APPLICATION_ARB) str << "Source : APPLICATION; ";
        else if (source == GL_DEBUG_SOURCE_OTHER_ARB) str << "Source : OTHER; ";

        if (type == GL_DEBUG_TYPE_ERROR_ARB) str << "Type : ERROR; ";
        else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB) str << "Type : DEPRECATED_BEHAVIOR; ";
        else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB) str << "Type : UNDEFINED_BEHAVIOR; ";
        else if (type == GL_DEBUG_TYPE_PORTABILITY_ARB) str << "Type : PORTABILITY; ";
        else if (type == GL_DEBUG_TYPE_PERFORMANCE_ARB) str << "Type : PERFORMANCE; ";
        else if (type == GL_DEBUG_TYPE_OTHER_ARB) str << "Type : OTHER; ";

        if (severity == GL_DEBUG_SEVERITY_HIGH_ARB) str << "Severity : HIGH; ";
        else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB) str << "Severity : MEDIUM; ";
        else if (severity == GL_DEBUG_SEVERITY_LOW_ARB) str << "Severity : LOW; ";

        // You can set a breakpoint here ! Your debugger will stop the program,
        // and the callstack will immediately show you the offending call.
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        LOG(GL_DEBUG) << converter.from_bytes(str.str()) << "Message : " << message << std::endl; // netbeans console output bug ...
    }

    /**
     * Creates a new windows GLWindow.
     * @param hInstance the applications instance
     * @param nCmdShow Controls how the window is to be shown (ignored).
     * @param title the windows title.
     * @param conf the configuration used
     */
    GLWindow::GLWindow(HINSTANCE hInstance, int nCmdShow, const std::string& title, Configuration& conf) :
        BaseGLWindow(conf.windowWidth, conf.windowHeight),
        hWnd(0),
        hDC(0),
        hRC(0),
        instance(hInstance),
        cmdShow(nCmdShow),
        windowClass("PatternsFWWindow"),
        windowTitle(title),
        config(conf),
        app(nullptr),
        pause(true),
        minimized(false),
        maximized(conf.fullscreen),
        resizing(false),
        frameCount(0)
    {
        this->InitWindow();
        this->InitOpenGL();
        if (config.useCUDA) this->InitCUDA();
    }

    GLWindow::~GLWindow()
    {
        if (config.useCUDA) this->ReleaseCUDA();
        this->ReleaseOpenGL();
        this->ReleaseWindow();
        config.fullscreen = maximized;
        config.windowWidth = fbo.GetWidth();
        config.windowHeight = fbo.GetHeight();
    }

    /**
     * Initializes the window.
     */
    void GLWindow::InitWindow()
    {
        RECT size = { config.windowLeft, config.windowTop,
            config.windowLeft + config.windowWidth, config.windowTop + config.windowHeight };
        DWORD dwExStyle;
        DWORD dwStyle;

        // Register class
        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wcex.lpfnWndProc = WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = instance;
        wcex.hIcon = LoadIcon(instance, (LPCTSTR)IDI_APPLICATION);
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = windowClass.c_str();
        wcex.hIconSm = LoadIcon(instance, (LPCTSTR)IDI_APPLICATION);
        if (!RegisterClassEx(&wcex)) {
            LOG(ERROR) << L"Could not register class!";
            throw std::runtime_error("Could not register class!");
        }

        if (maximized) // Attempt Fullscreen Mode?
        {
            DEVMODE dmScreenSettings; // Device Mode
            memset(&dmScreenSettings, 0, sizeof(dmScreenSettings)); // Makes Sure Memory's Cleared
            dmScreenSettings.dmSize = sizeof(dmScreenSettings); // Size Of The Devmode Structure
            dmScreenSettings.dmPelsWidth = size.right - size.left; // Selected Screen Width
            dmScreenSettings.dmPelsHeight = size.bottom - size.top; // Selected Screen Height
            dmScreenSettings.dmBitsPerPel = config.backbufferBits; // Selected Bits Per Pixel
            dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

            // Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
            if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
                // If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
                if (MessageBox(NULL, "The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?",
                    windowClass.c_str(), MB_YESNO | MB_ICONEXCLAMATION) == IDYES) {
                    config.fullscreen = false;
                    maximized = false;
                } else {
                    LOG(ERROR) << L"Program Will Now Close.";
                    throw std::runtime_error("User requested abort.");
                }
            }
        }

        if (maximized) // Are We Still In Fullscreen Mode?
        {
            dwExStyle = WS_EX_APPWINDOW; // Window Extended Style
            dwStyle = WS_POPUP; // Windows Style
            ShowCursor(FALSE); // Hide Mouse Pointer
        } else {
            dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; // Window Extended Style
            dwStyle = WS_OVERLAPPEDWINDOW; // Windows Style
        }

        AdjustWindowRectEx(&size, dwStyle, FALSE, dwExStyle);

        // Create window
        hWnd = CreateWindowEx(dwExStyle, windowClass.c_str(), windowTitle.c_str(),
            dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
            config.windowLeft, config.windowTop,
            size.right - size.left, size.bottom - size.top,
            NULL, // No Parent Window
            NULL, // No Menu
            instance, (void*) this);
        if (!hWnd) {
            this->ReleaseWindow();
            LOG(ERROR) << L"Could not create window!";
            throw std::runtime_error("Could not create window!");
        }

        LOG(DEBUG) << L"Window successfully initialized.";
    }

    /**
     * Initializes OpenGL.
     */
    void GLWindow::InitOpenGL()
    {
        hDC = GetDC(hWnd);
        if (!(hDC)) {
            this->ReleaseOpenGL();
            LOG(ERROR) << L"Can't create a GDI Device Context.";
            throw std::runtime_error("Can't create a GDI Device Context.");
        }

        PIXELFORMATDESCRIPTOR pfd = {
            sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            24, //Number of bits for the depthbuffer
            8, //Number of bits for the stencilbuffer
            0, //Number of Aux buffers in the framebuffer.
            PFD_MAIN_PLANE, 0, 0, 0, 0
        };

        GLuint pixelFormat = ChoosePixelFormat(hDC, &pfd);
        if (!pixelFormat) {
            this->ReleaseOpenGL();
            LOG(ERROR) << L"Can't find a suitable (dummy) PixelFormat.";
            throw std::runtime_error("Can't find a suitable (dummy) PixelFormat.");
        }

        if (!SetPixelFormat(hDC, pixelFormat, &pfd)) {
            this->ReleaseOpenGL();
            LOG(ERROR) << L"Can't set the (dummy) PixelFormat.";
            throw std::runtime_error("Can't set the (dummy) PixelFormat.");
        }

        hRC = wglCreateContext(hDC);
        if (!hRC) {
            this->ReleaseOpenGL();
            LOG(ERROR) << L"Can't create a (dummy) GL rendering context.";
            throw std::runtime_error("Can't create a (dummy) GL rendering context.");
        }

        if (!wglMakeCurrent(hDC, hRC)) {
            this->ReleaseOpenGL();
            LOG(ERROR) << L"Can't activate the (dummy) GL rendering context.";
            throw std::runtime_error("Can't activate the (dummy) GL rendering context.");
        }

        glewExperimental = GL_TRUE;
        LOG(INFO) << L"Initializing GLEW...";
        if (GLEW_OK != glewInit()) {
            this->ReleaseOpenGL();
            LOG(ERROR) << L"Could not initialize GLEW!";
            throw std::runtime_error("Could not initialize GLEW!");
        }

        // TODO: higher opengl version?
        LOG(INFO) << L"Checking OpenGL version 3.3 ...";
        if (!GLEW_VERSION_3_3) {
            this->ReleaseOpenGL();
            LOG(ERROR) << L"OpenGL version not supported.";
            throw std::runtime_error("OpenGL version not supported.");
        }

        if (!(WGLEW_ARB_pixel_format && WGLEW_ARB_create_context)) {
            // no need to get the better context.
            return;
        }

        // destroy dummy context / window.
        this->ReleaseOpenGL();
        this->ReleaseWindow();

        // Create the window again
        this->InitWindow();

        hDC = GetDC(hWnd);
        if (!hDC) {
            this->ReleaseOpenGL();
            LOG(ERROR) << L"Can't create a GDI Device Context.";
            throw std::runtime_error("Can't create a GDI Device Context.");
        }

        unsigned int nPixCount = 0;
        // Specify the important attributes we care about
        int pixAttribs[] = {
            WGL_SUPPORT_OPENGL_ARB, 1, // Must support OGL rendering
            WGL_DRAW_TO_WINDOW_ARB, 1, // pf that can run a window
            WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB, // must be HW accelerated
            WGL_COLOR_BITS_ARB, 24, // 8 bits of each R, G and B
            WGL_DEPTH_BITS_ARB, 16, // 16 bits of depth precision for window
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE, // Double buffered context
            WGL_SAMPLE_BUFFERS_ARB, GL_TRUE, // MSAA on
            WGL_SAMPLES_ARB, 8, // 8x MSAA
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB, // pf should be RGBA type
            0 // NULL termination
        };
        // Ask OpenGL to find the most relevant format matching our attribs
        // Only get one format back.
        int nPF;
        if (FALSE == wglChoosePixelFormatARB(hDC, pixAttribs, NULL, 1, &nPF, &nPixCount) || nPF == -1) {
            this->ReleaseOpenGL();
            LOG(ERROR) << L"Can't find a suitable PixelFormat.";
            throw std::runtime_error("Can't find a suitable PixelFormat.");
        }

        if (!SetPixelFormat(hDC, nPF, &pfd)) {
            this->ReleaseOpenGL();
            LOG(ERROR) << L"Can't set the (final) PixelFormat.";
            throw std::runtime_error("Can't set the (final) PixelFormat.");
        }


        int attribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, PTRN_OPENGL_MAJOR_VERSION,
            WGL_CONTEXT_MINOR_VERSION_ARB, PTRN_OPENGL_MINOR_VERSION,
#ifdef _DEBUG
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | WGL_CONTEXT_DEBUG_BIT_ARB,
#else
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
#endif
            // WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            0
        };

        hRC = wglCreateContextAttribsARB(hDC, 0, attribs);
        if (hRC == NULL) {
            DWORD err = GetLastError();
            this->ReleaseOpenGL();
            LOG(ERROR) << L"Could not create rendering context: " << err;
            throw std::runtime_error("Could not create rendering context.");
        }

        if (!wglMakeCurrent(hDC, hRC)) {
            this->ReleaseOpenGL();
            LOG(ERROR) << L"Can't activate the (final) GL rendering context.";
            throw std::runtime_error("Can't activate the (final) GL rendering context.");
        }
        LOG(INFO) << L"OpenGL context initialized.";

#ifdef _DEBUG
        // get the debug output extension by hand ...
        int NumberOfExtensions;
        bool debugOutput = false;
        glGetIntegerv(GL_NUM_EXTENSIONS, &NumberOfExtensions);
        for (int i = 0; i < NumberOfExtensions; i++) {
            const GLubyte *ccc = glGetStringi(GL_EXTENSIONS, i);
            if (strcmp(reinterpret_cast<const char*> (ccc), "GL_ARB_debug_output") == 0) {
                // The extension is supported by our hardware and driver
                // Try to get the "glDebugMessageCallbackARB" function :
                glDebugMessageCallback =
                    (PFNGLDEBUGMESSAGECALLBACKPROC)wglGetProcAddress("glDebugMessageCallback");
                glDebugMessageControl =
                    (PFNGLDEBUGMESSAGECONTROLPROC)wglGetProcAddress("glDebugMessageControl");
                glDebugMessageInsert =
                    (PFNGLDEBUGMESSAGEINSERTPROC)wglGetProcAddress("glDebugMessageInsert");
                LOG(DEBUG) << L"The OpenGL implementation provides debug output.";
                glDebugMessageCallback(&DebugOutputCallback, nullptr);
                glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
                GLuint unusedIds = 0;
                glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, true);
                glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION_ARB, GL_DEBUG_TYPE_OTHER_ARB,
                    1, GL_DEBUG_SEVERITY_HIGH, -1, "OpenGL Debug Log here ...");
                debugOutput = true;
            }
        }
        if (!debugOutput) {
            LOG(DEBUG) << L"The OpenGL implementation does not provide debug output.";
        }
#endif

        fbo.Resize(config.windowWidth, config.windowHeight);

        if (!GLEW_ARB_vertex_array_object) {
            LOG(WARNING) << L"VAOs not supported ...";
        }
    }

    void GLWindow::InitCUDA()
    {
        cudaError_t result;
        if (config.cudaDevice == -1) {
            unsigned int deviceCount;
            int devices;
            result = cudaGLGetDevices(&deviceCount, &devices, 1, cudaGLDeviceListAll);
            if (result != cudaSuccess) {
                LOG(ERROR) << L"Could not get CUDA GL devices. Error: " << result;
                throw std::runtime_error("Could not get CUDA GL devices.");
            }

            if (deviceCount == 0) {
                LOG(ERROR) << L"No CUDA devices found.";
                throw std::runtime_error("No CUDA devices found.");
            }
            config.cudaDevice = devices;
        }

        result = cudaSetDevice(config.cudaDevice);
        if (result != cudaSuccess) {
            LOG(ERROR) << L"Could not set CUDA device. Error: " << result;
            config.cudaDevice = -1;
            throw std::runtime_error("Could not set CUDA device.");
        }
        cudaDeviceReset();
        cudaDeviceSynchronize();
    }

    /**
     * Registers the application object using the window for event management.
     * @param application the application object
     */
    void GLWindow::RegisterApplication(ApplicationBase & application)
    {
        this->app = &application;
        texManager = app->GetTextureManager();
        matManager = app->GetMaterialLibManager();
        shaderManager = app->GetShaderManager();
        programManager = app->GetGPUProgramManager();
        uboBindingPoints = app->GetUBOBindingPoints();
    }

    /**
     * Shows the window.
     */
    void GLWindow::ShowWindow()
    {
        ::ShowWindow(hWnd, cmdShow);
        SetForegroundWindow(hWnd);
        SetFocus(hWnd);
    }

    void GLWindow::CloseWindow()
    {
        SendMessage(this->hWnd, WM_CLOSE, 0, 0);
    }

    void GLWindow::ReleaseCUDA()
    {
        cudaError_t result = cudaDeviceReset();
        if (result != cudaSuccess) {
            LOG(ERROR) << L"Could not reset CUDA device. Error: " << result;
        }
    }

    void GLWindow::ReleaseOpenGL()
    {
        if (maximized) {
            ChangeDisplaySettings(NULL, 0);
            ShowCursor(TRUE);
        }

        if (hRC) {
            if (!wglMakeCurrent(hDC, NULL)) {
                LOG(ERROR) << L"Release of DC and RC failed.";
            }

            if (!wglDeleteContext(hRC)) {
                LOG(ERROR) << L"Release rendering context failed.";
            }
            hRC = 0;
        }

        if (hDC && !ReleaseDC(hWnd, hDC)) {
            LOG(ERROR) << L"Release device context failed.";
            hDC = NULL;
        }
    }

    void GLWindow::ReleaseWindow()
    {
        if (hWnd && !DestroyWindow(hWnd)) {
            LOG(ERROR) << L"Could Not Release hWnd.";
            hWnd = 0;
        }

        if (instance && !UnregisterClassA(this->windowClass.c_str(), this->instance)) {

            LOG(ERROR) << L"Could Not Unregister Class.";
            instance = 0;
        }
    }

    void GLWindow::Present()
    {
        glFlush();
        SwapBuffers(this->hDC);
    }

    bool GLWindow::MessageBoxQuestion(const std::string& title, const std::string& content)
    {
        return MessageBoxA(hWnd, content.c_str(), title.c_str(), MB_YESNO) == IDYES;
    }

    /**
     * @brief Handles the resize operation of the application.
     *
     * @author Sebastian Maisch
     * @date   25.06.2012
     *
     * @return The methods error state.
     */
    void GLWindow::HandleResize()
    {
        assert(this->app != nullptr);
        LOG(DEBUG) << L"Begin HandleResize()";
        unsigned int width = this->maximized ? GetSystemMetrics(SM_CXSCREEN) : this->width;
        unsigned int height = this->maximized ? GetSystemMetrics(SM_CYSCREEN) : this->height;

        if (width == 0 || height == 0) {
            return;
        }
        this->Resize(width, height);

        if (this->hWnd != 0) {
            try {
                this->app->OnResize(width, height);
            }
            catch (std::runtime_error e) {

                LOG(ERROR) << L"Could not reacquire resources after resize: " << e.what();
                throw std::runtime_error("Could not reacquire resources after resize.");
            }
        }

        this->config.windowWidth = width;
        this->config.windowHeight = height;
    }

    /**
     * Handle the windows messages.
     * @param message the message send
     * @param wParam the wParam
     * @param lParam the lParam
     * @return the handling result
     */
    LRESULT GLWindow::HandleMessages(UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_SIZE:
            if (this->app != nullptr) {
                HandleSizeEvent(wParam);
            }
            break;
        case WM_ENTERSIZEMOVE:
        {
            LOG(DEBUG) << L"Starting resize / moving the window.";
            this->pause = true;
            this->resizing = true;
        }
        break;
        case WM_EXITSIZEMOVE:
        {
            LOG(DEBUG) << L"Finished resize / moving the window. Handling resize.";
            this->pause = false;
            this->resizing = false;
            if (this->app != nullptr) {
                this->HandleResize();
            }
        }
        break;
        case WM_SYSCOMMAND:
            // Prevent moving/sizing in full screen mode
            switch ((wParam & 0xFFF0))
            {
            case SC_MOVE:
            case SC_SIZE:
            case SC_MAXIMIZE:
            case SC_KEYMENU:
                if (this->maximized || this->minimized)
                    return 0;
                break;
            }
            break;
        case WM_KILLFOCUS:
            this->pause = GetConfig().pauseOnKillFocus;
            break;
        case WM_SETFOCUS:
            if (GetConfig().pauseOnKillFocus) this->pause = false;
            mouseButtonState = 0;
            keyboardModState = 0;
            break;
        case WM_MOUSEMOVE:
            mouseAbsolute = glm::vec2(static_cast<float>((short)LOWORD(lParam)), static_cast<float>((short)HIWORD(lParam)));
            hadPositionUpdate = true;
            break;
        }

        return DefWindowProc(this->hWnd, message, wParam, lParam);
    }

    void GLWindow::HandleRawKeyboard(const RAWKEYBOARD& raw)
    {
        // remapping stolen from https://molecularmusings.wordpress.com/2011/09/05/properly-handling-keyboard-input/
        unsigned int vkCode = raw.VKey;
        unsigned int scanCode = raw.MakeCode;
        if (vkCode == 255) return;

        if (vkCode == VK_SHIFT) vkCode = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX);
        else if (vkCode == VK_NUMLOCK) scanCode = (MapVirtualKey(vkCode, MAPVK_VK_TO_VSC) | 0x100);

        const bool isE0 = ((raw.Flags & RI_KEY_E0) != 0);
        const bool isE1 = ((raw.Flags & RI_KEY_E1) != 0);

        if (isE1)
        {
            if (vkCode == VK_PAUSE) scanCode = 0x45;
            else scanCode = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC);
        }

        switch (vkCode)
        {
        case VK_CONTROL:
            if (isE0) vkCode = VK_RCONTROL;
            else vkCode = VK_LCONTROL;
            break;

        case VK_MENU:
            if (isE0) vkCode = VK_RMENU;
            else vkCode = VK_LMENU;
            break;

        case VK_RETURN:
            if (isE0) vkCode = VK_NUMRETURN;
            break;

        case VK_INSERT:
            if (!isE0) vkCode = VK_NUMPAD0;
            break;

        case VK_DELETE:
            if (!isE0) vkCode = VK_DECIMAL;
            break;

        case VK_HOME:
            if (!isE0) vkCode = VK_NUMPAD7;
            break;

        case VK_END:
            if (!isE0) vkCode = VK_NUMPAD1;
            break;

        case VK_PRIOR:
            if (!isE0) vkCode = VK_NUMPAD9;
            break;

        case VK_NEXT:
            if (!isE0) vkCode = VK_NUMPAD3;
            break;

        case VK_LEFT:
            if (!isE0) vkCode = VK_NUMPAD4;
            break;

        case VK_RIGHT:
            if (!isE0) vkCode = VK_NUMPAD6;
            break;

        case VK_UP:
            if (!isE0) vkCode = VK_NUMPAD8;
            break;

        case VK_DOWN:
            if (!isE0) vkCode = VK_NUMPAD2;
            break;

        case VK_CLEAR:
            if (!isE0) vkCode = VK_NUMPAD5;
            break;
        }

        /*UINT key = (scanCode << 16) | (isE0 << 24);
        char buffer[512] = {};
        GetKeyNameText((LONG)key, buffer, 512);
        LOG(DEBUG) << "Key Event: " << buffer << "; VKCODE: " << std::hex << std::showbase
        << std::internal << std::setfill(L'0') << std::setw(4) << vkCode << " ScanCode: "
        << scanCode << "(" << isE0 << "|" << isE1 << ")";*/

        if ((vkCode == VK_LSHIFT || vkCode == VK_RSHIFT || vkCode == VK_SHIFT) && raw.Flags & RI_KEY_MAKE) keyboardModState |= VK_MOD_SHIFT;
        else if ((vkCode == VK_LSHIFT || vkCode == VK_RSHIFT || vkCode == VK_SHIFT) && raw.Flags & RI_KEY_BREAK) keyboardModState &= ~VK_MOD_SHIFT;
        else if ((vkCode == VK_LCONTROL || vkCode == VK_RCONTROL || vkCode == VK_CONTROL) && raw.Flags & RI_KEY_MAKE) keyboardModState |= VK_MOD_CTRL;
        else if ((vkCode == VK_LCONTROL || vkCode == VK_RCONTROL || vkCode == VK_CONTROL) && raw.Flags & RI_KEY_BREAK) keyboardModState &= ~VK_MOD_CTRL;
        else if ((vkCode == VK_LMENU || vkCode == VK_RMENU || vkCode == VK_MENU) && raw.Flags & RI_KEY_MAKE) keyboardModState |= VK_MOD_MENU;
        else if ((vkCode == VK_LMENU || vkCode == VK_RMENU || vkCode == VK_MENU) && raw.Flags & RI_KEY_BREAK) keyboardModState &= ~VK_MOD_MENU;

        if (this->app != nullptr) {
            bool keyDown = false;
            if (!(raw.Flags & RI_KEY_BREAK)) keyDown = true;
            //else if (!(raw.Flags & RI_KEY_BREAK)) {
            //    LOG(DEBUG) << "KeyEvent " << std::hex << std::showbase << std::internal << std::setfill(L'0') << std::setw(4) << vkCode << " omitted. (" << isE0 << " | " << isE1 << ")";
            //    return; // if no pressed or released event -> skip
            //}
            // LOG(DEBUG) << "KeyEvent " << std::hex << std::showbase << std::internal << std::setfill(L'0') << std::setw(4) << vkCode << ", " << keyboardModState << ", " << keyDown << ", " << raw.Flags;
            this->app->HandleKeyboard(vkCode, keyDown, this);
        }
    }

    void GLWindow::HandleRawMouse(const RAWMOUSE& raw)
    {
        mouseRelative = glm::vec2(static_cast<float>(raw.lLastX), static_cast<float>(raw.lLastY));

        if (raw.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) {
            mouseButtonState |= MB_LEFT; // LOG(DEBUG) << "Left Mouse Down. STATE: " << mouseButtonState;
        }
        if (raw.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) {
            mouseButtonState &= ~MB_LEFT; // LOG(DEBUG) << "Left Mouse Up. STATE: " << mouseButtonState;
        }

        if (raw.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) mouseButtonState |= MB_RGHT;
        if (raw.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) mouseButtonState &= ~MB_RGHT;

        if (raw.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) mouseButtonState |= MB_MIDD;
        if (raw.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP) mouseButtonState &= ~MB_MIDD;

        if (raw.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN) mouseButtonState |= MB_BTN4;
        if (raw.usButtonFlags & RI_MOUSE_BUTTON_4_UP) mouseButtonState &= ~MB_BTN4;
        if (raw.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN) mouseButtonState |= MB_BTN5;
        if (raw.usButtonFlags & RI_MOUSE_BUTTON_5_UP) mouseButtonState &= ~MB_BTN5;

        float mouseWheelDelta = 0.0f;
        if (raw.usButtonFlags & RI_MOUSE_WHEEL) mouseWheelDelta = static_cast<float>((short)raw.usButtonData);

        if (this->app != nullptr) {
            this->app->HandleMouse(raw.usButtonFlags, mouseWheelDelta, this);
        }
    }

    /**
     * @brief Handles the size event described by wParam.
     *
     * @ingroup debug
     * @author Sebastian Maisch
     * @date   25.06.2012
     *
     * @param  wParam  The wParam field of the message.
     */
    void GLWindow::HandleSizeEvent(WPARAM wParam)
    {
        assert(this->app != nullptr);

        POINT mousePt;
        GetCursorPos(&mousePt);
        ScreenToClient(hWnd, &mousePt);

        if (wParam == SIZE_MINIMIZED) {
            this->pause = true;
            this->minimized = true;
            this->maximized = false;
            return;
        }

        RECT rcCurrentClient;
        GetClientRect(this->hWnd, &rcCurrentClient);
        width = rcCurrentClient.right - rcCurrentClient.left;
        height = rcCurrentClient.bottom - rcCurrentClient.top;
        if (rcCurrentClient.top == 0 && rcCurrentClient.bottom == 0) {
        } else if (wParam == SIZE_MAXIMIZED) {
            if (this->minimized) this->pause = false;
            this->minimized = false;
            this->maximized = true;
            this->HandleResize();
        } else if (wParam == SIZE_RESTORED) {
            if (this->resizing) return;

            if (this->minimized) this->pause = false;
            this->minimized = false;
            this->maximized = false;
            this->HandleResize();
        }
    }

    /**
     * Returns the current configuration.
     * @return the configuration
     */
    Configuration& GLWindow::GetConfig()
    {
        return config;
    }

    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message == WM_CREATE) { // Store pointer to RenderWindow in user data area
            SetWindowLongPtr(hWnd, GWLP_USERDATA,
                (LONG_PTR)(((LPCREATESTRUCT)lParam)->lpCreateParams));

            RAWINPUTDEVICE rid[2];
            rid[0].dwFlags = 0;
            rid[0].usUsagePage = 0x01;
            rid[0].usUsage = 0x06;
            rid[0].hwndTarget = hWnd;
            rid[1].dwFlags = 0;
            rid[1].usUsagePage = 0x01;
            rid[1].usUsage = 0x02;
            rid[1].hwndTarget = hWnd;
            RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE));

            return 0;
        }

        // look up window instance
        // note: it is possible to get a WM_SIZE before WM_CREATE
        GLWindow* win = (GLWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
        if (!win)
            return DefWindowProc(hWnd, message, wParam, lParam);

        PAINTSTRUCT ps;

        switch (message)
        {
        case WM_INPUT:
        {
            UINT dwSize = 0;
            if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER)) == -1){
                break;
            }
            std::vector<BYTE> buffer(dwSize);
            if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer.data(), &dwSize, sizeof(RAWINPUTHEADER)) != dwSize){
                break;
            }

            RAWINPUT *raw = (RAWINPUT*)buffer.data();
            if (raw->header.dwType == RIM_TYPEKEYBOARD) {
                win->HandleRawKeyboard(raw->data.keyboard);
            } else if (raw->header.dwType == RIM_TYPEMOUSE) {
                win->HandleRawMouse(raw->data.mouse);
            }
            DefRawInputProc(&raw, 1, sizeof(RAWINPUTHEADER));
        } break;
        case WM_PAINT:
            BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_CLOSE:
            // case WM_DESTROY:
            LOG(INFO) << L"Got quit message ...";
            PostQuitMessage(0);
            break;

        default:
            return win->HandleMessages(message, wParam, lParam);
        }
        return 0;
    }
}

#endif /* _WIN32 */
