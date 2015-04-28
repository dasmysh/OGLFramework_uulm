/**
 * @file   GLWindow.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   18. Dezember 2013
 * @ingroup win
 *
 * @brief  Windows declaration for the GLWindow.
 */

#ifndef GLWINDOW_H
#define GLWINDOW_H

#include "main.h"
#include "Configuration.h"
#include "BaseGLWindow.h"

class ApplicationBase;

/**
 * @brief Windows declaration for the GLWindow.
 * @ingroup win
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   18. Dezember 2013
 */
class GLWindow : public BaseGLWindow
{
public:
    GLWindow(HINSTANCE hInstance, int nCmdShow, const std::string& title, Configuration& conf);
    virtual ~GLWindow();

    void ShowWindow();
    void CloseWindow();
    void RegisterApplication(ApplicationBase& application);
    void Present();
    bool MessageBoxQuestion(const std::string& title, const std::string content);

    LRESULT HandleMessages(UINT message, WPARAM wParam, LPARAM lParam);
    Configuration& GetConfig();

private:
    HWND hWnd;
    HDC hDC;
    HGLRC hRC;
    HINSTANCE instance;
    int cmdShow;
    std::string windowClass;
    std::string windowTitle;
    Configuration& config;
    ApplicationBase* app;

    // window status
    /// <summary>   true if rendering is paused. </summary>
    bool pause;
    /// <summary>   true if minimized. </summary>
    bool minimized;
    /// <summary>   true if maximized. </summary>
    bool maximized;
    /// <summary>   true if window is resizing. </summary>
    bool resizing;
    /// The number (id) of the current frame.
    unsigned int frameCount;

    void InitWindow();
    void InitOpenGL();
    void InitCUDA();
    void ReleaseWindow();
    void ReleaseOpenGL();
    void ReleaseCUDA();
    void HandleResize();

    void CheckForKeyboardMessages(UINT message, LPARAM lParam, WPARAM wParam);
    void HandleSizeEvent(WPARAM wParam);
    void CheckForMouseMessages(UINT message, LPARAM lParam, WPARAM wParam);

    bool isMouseEvent(UINT message);

};

#endif /* GLWINDOW_H */
