#include "../window_handle.h"

#include <iostream>

#include "imgui.h"
#include "uiaccess/uiaccess.h"

constexpr auto WINDOW_CLASS_NAME = L"TopmostTransparentOverlayClass";
constexpr auto WINDOW_NAME = L"Merkur Overlay";

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef LRESULT(CALLBACK *PFN_DefWindowProc)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

static PFN_DefWindowProc old_handler = DefWindowProc;

static LRESULT WINAPI WndProc_custom(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // Forward to ImGui first
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    // Handle your other window messages or forward to original
    return old_handler(hWnd, msg, wParam, lParam);
}

BOOL update_wnd_proc(HWND handle)
{
    if (!handle)
    {
        std::cerr << "Error: handle is null." << std::endl;
        return FALSE;
    }

    old_handler = (PFN_DefWindowProc)SetWindowLongPtr(handle, GWLP_WNDPROC, (LONG_PTR)WndProc_custom);

    if (old_handler == nullptr)
    {
        std::cerr << "Error: Failed to update WndProc handler: " << GetLastError() << std::endl;
        return false;
    }

    return true;
}

static ATOM get_window_class()
{
    WNDCLASSEX wc = {
        .cbSize = sizeof(wc),
        .style = CS_CLASSDC,
        .lpfnWndProc = DefWindowProc,
        .cbClsExtra = 0L,
        .cbWndExtra = 0L,
        .hInstance = GetModuleHandle(NULL),
        .hIcon = NULL,
        .hCursor = LoadCursor(nullptr, IDC_ARROW),
        .hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH),
        .lpszMenuName = NULL,
        .lpszClassName = WINDOW_CLASS_NAME,
        .hIconSm = NULL,
    };

    return RegisterClassEx(&wc);
}

static void configure_window(HWND handle)
{
    auto style = GetWindowLongPtr(handle, GWL_EXSTYLE);
    style |= WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
    SetWindowLongPtr(handle, GWL_EXSTYLE, style);

    // avoiding window flags: https://www.unknowncheats.me/forum/general-programming-and-reversing/229070-overlay-window-topmost-top-game-windows-video.html
    MARGINS margins = {
        .cxLeftWidth = -1,
        .cxRightWidth = -1,
        .cyTopHeight = -1,
        .cyBottomHeight = -1,
    };

    DwmExtendFrameIntoClientArea(handle, &margins);
    SetLayeredWindowAttributes(handle, 0, 0xFF, LWA_ALPHA);
    SetWindowPos(handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

BOOL use_uiaccess(HWND *pHandle)
{

    typedef HWND(WINAPI * CreateWindowInBand)(
        DWORD dwExStyle,
        LPCWSTR lpClassName,
        LPCWSTR lpWindowName,
        DWORD dwStyle,
        int x,
        int y,
        int nWidth,
        int nHeight,
        HWND hWndParent,
        HMENU hMenu,
        HINSTANCE hInstance,
        LPVOID lpParam,
        DWORD dwBand);

    constexpr auto ZBID_UIACCESS = 2; // UIAccess Band ID

    const DWORD dwErr = PrepareForUIAccess();
    if (dwErr != ERROR_SUCCESS)
        return dwErr;

    std::cout << "Process has UI Access." << std::endl;

    const auto user32_dll = LoadLibrary(L"user32.dll");
    if (user32_dll == 0)
    {
        std::cerr << "Failed to load user32.dll: " << GetLastError() << std::endl;
        return FALSE;
    }

    const auto pCreateWindowInBand = (CreateWindowInBand)GetProcAddress(user32_dll, "CreateWindowInBand");
    if (pCreateWindowInBand == nullptr)
    {
        std::cerr << "Failed to get CreateWindowInBand function address: " << GetLastError() << std::endl;
        return FALSE;
    }

    const auto atom = get_window_class();
    if (atom == 0)
    {
        std::cerr << "Failed to register window class." << std::endl;
        return FALSE;
    }

    HWND handle = pCreateWindowInBand(
        0,
        MAKEINTATOM(atom),
        WINDOW_NAME,
        WS_POPUP,
        0, 0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        NULL,
        NULL,
        GetModuleHandle(NULL),
        LPVOID(atom),
        ZBID_UIACCESS);

    if (handle == 0)
    {
        std::cerr << "Failed to create window in band: " << GetLastError() << std::endl;
        return FALSE;
    }

    configure_window(handle);

    *pHandle = handle;

    return true;
}

BOOL hook_nvidia(HWND *pHandle)
{
    HWND nvidia_handle = FindWindow(L"CEF-OSC-WIDGET", L"NVIDIA GeForce Overlay");

    if (!nvidia_handle)
    {
        std::cerr << "Failed to find NVIDIA Overlay window." << std::endl;
        return false;
    }

    const auto atom = get_window_class();
    if (atom == 0)
    {
        std::cerr << "Failed to register window class." << std::endl;
        return FALSE;
    }

    HWND handle = CreateWindowEx(
        0,
        MAKEINTATOM(atom),
        WINDOW_NAME,
        WS_POPUP,
        0, 0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        nvidia_handle,
        nullptr,
        GetModuleHandle(NULL),
        LPVOID(atom));

    if (!handle)
    {
        const auto error = GetLastError();
        std::cerr << "Failed to create window: " << error << std::endl;
        return FALSE;
    }

    configure_window(handle);

    *pHandle = handle;

    return true;
}

BOOL new_window(HWND *pHandle)
{
    const auto atom = get_window_class();
    if (atom == 0)
    {
        std::cerr << "Failed to register window class." << std::endl;
        return FALSE;
    }

    HWND handle = CreateWindowEx(
        0,
        MAKEINTATOM(atom),
        WINDOW_NAME,
        WS_POPUP,
        0, 0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        nullptr,
        nullptr,
        GetModuleHandle(NULL),
        LPVOID(atom));

    if (!handle)
    {
        const auto error = GetLastError();
        std::cerr << "Failed to create window: " << error << std::endl;
        return FALSE;
    }

    configure_window(handle);

    *pHandle = handle;

    return true;
}
