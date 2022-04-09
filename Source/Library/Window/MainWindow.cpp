#include "Window/MainWindow.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::Initialize

      Summary:  Initializes main window

      Args:     HINSTANCE hInstance
                  Handle to the instance
                INT nCmdShow
                  Is a flag that says whether the main application window
                  will be minimized, maximized, or shown normally
                PCWSTR pszWindowName
                  The window name

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT MainWindow::Initialize(_In_ HINSTANCE hInstance, _In_ INT nCmdShow, _In_ PCWSTR pszWindowName)
    {
        RECT rc = { 0, 0, 800, 600 };
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

        return initialize(hInstance, nCmdShow, pszWindowName, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top);
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::GetWindowClassName

      Summary:  Returns the name of the window class

      Returns:  PCWSTR
                  Name of the window class
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    PCWSTR MainWindow::GetWindowClassName() const
    {
        return L"TutorialWindowClass";
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::HandleMessage

      Summary:  Handles the messages

      Args:     UINT uMessage
                  Message code
                WPARAM wParam
                  Additional data the pertains to the message
                LPARAM lParam
                  Additional data the pertains to the message

      Returns:  LRESULT
                  Integer value that your program returns to Windows
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    LRESULT MainWindow::HandleMessage(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
    {
        PAINTSTRUCT ps;
        HDC hdc;
        WCHAR szDebugMessage[64];

        // mouse raw input
        RAWINPUTDEVICE Rid[1];
        Rid[0].usUsagePage = (USHORT)0x01;
        Rid[0].usUsage = (USHORT)0x02;
        Rid[0].dwFlags = RIDEV_INPUTSINK;
        Rid[0].hwndTarget = m_hWnd;

        RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

        switch (uMsg)
        {
        case WM_INPUT:
        {
            UINT dwSize = sizeof(RAWINPUT);
            static BYTE lpb[sizeof(RAWINPUT)];

            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

            RAWINPUT* raw = (RAWINPUT*)lpb;

            if (raw->header.dwType == RIM_TYPEMOUSE)
            {
                m_mouseRelativeMovement.X = raw->data.mouse.lLastX;
                m_mouseRelativeMovement.Y = raw->data.mouse.lLastY;
            }
            break;
        }
        case WM_KEYDOWN:
        {
            // Set directions to true
            switch (wParam)
            {
            case 0x57: // W - Front
                m_directions.bFront = TRUE;
                break;
            case 0x41: // A - Left
                m_directions.bLeft = TRUE;
                break;
            case 0x53: // S - Back
                m_directions.bBack = TRUE;
                break;
            case 0x44: // D - Right
                m_directions.bRight = TRUE;
                break;
            case VK_SHIFT: // Left Shift. Down
                m_directions.bDown = TRUE;
                break;
            case VK_SPACE: // Space Bar. Up
                m_directions.bUp = TRUE;
                break;
            }
            return 0;
        }
        case WM_KEYUP:
        {
            // set directions to false
            switch (wParam)
            {
            case 0x57: // W - Front
                m_directions.bFront = FALSE;
                break;
            case 0x41: // A - Left
                m_directions.bLeft = FALSE;
                break;
            case 0x53: // S - Back
                m_directions.bBack = FALSE;
                break;
            case 0x44: // D - Right
                m_directions.bRight = FALSE;
                break;
            case VK_SHIFT: // Left Shift. Down
                m_directions.bDown = FALSE;
                break;
            case VK_SPACE: // Space Bar. Up
                m_directions.bUp = FALSE;
                break;
            }
            return 0;
        }
        case WM_PAINT:
        {
            hdc = BeginPaint(m_hWnd, &ps);
            EndPaint(m_hWnd, &ps);
            break;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        default:
            return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
        }

        return 0;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::GetDirections

      Summary:  Returns the keyboard direction input

      Returns:  const DirectionsInput&
                  Keyboard direction input
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    const DirectionsInput& MainWindow::GetDirections() const
    {
        return m_directions;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::GetMouseRelativeMovement

      Summary:  Returns the mouse relative movement

      Returns:  const MouseRelativeMovement&
                  Mouse relative movement
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    const MouseRelativeMovement& MainWindow::GetMouseRelativeMovement() const
    {
        return m_mouseRelativeMovement;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::ResetMouseMovement

      Summary:  Reset the mouse relative movement to zero
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void MainWindow::ResetMouseMovement()
    {
        m_mouseRelativeMovement = {0, };
    }
}
