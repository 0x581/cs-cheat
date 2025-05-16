#include "../window_handle.h"

#include <iostream>

#include "../../lib/imgui/pch.h"
#include "../../lib/uiaccess/uiaccess.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc_custom(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	// Forward to ImGui first
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	// Handle your other window messages or forward to original
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


BOOL use_uiaccess(HWND* pHandle) {

	typedef HWND(WINAPI* CreateWindowInBand)(
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
		DWORD dwBand
		);

	//CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	DWORD dwErr = PrepareForUIAccess();

	if (dwErr != ERROR_SUCCESS)
		return dwErr;

	std::cout << "Process has UI Access." << std::endl;

	constexpr auto ZBID_UIACCESS = 2; // UIAccess Band ID

	const auto user32_dll = LoadLibrary(L"user32.dll");
	if (user32_dll == 0)
	{
		std::cout << "Failed to load user32.dll" << std::endl;
		return FALSE;
	}

	const auto pCreateWindowInBand = (CreateWindowInBand)GetProcAddress(user32_dll, "CreateWindowInBand");

	WNDCLASSEXW wc = {
		sizeof(wc),
		CS_CLASSDC,
		DefWindowProc,
		0L,
		0L,
		GetModuleHandle(NULL),
		NULL,
		NULL,
		NULL,
		NULL,
		L"ImGui Example",
		NULL,
	};
	auto res = RegisterClassExW(&wc);

	HWND hwnd = pCreateWindowInBand(
		WS_EX_TOPMOST | WS_EX_NOACTIVATE,
		(LPCWSTR)res,
		NULL,
		0x80000000,
		0,
		0,
		0,
		0,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		LPVOID(res),
		ZBID_UIACCESS
	);

	if (hwnd == 0)
	{
		const auto error = GetLastError();
		std::cout << "Failed to create window in band: " << error << std::endl;
		return FALSE;
	}

	*pHandle = hwnd;

	return hwnd != NULL;
}


BOOL hook_nvidia(HWND* pHandle) {
	HWND handle = FindWindow(L"CEF-OSC-WIDGET", L"NVIDIA GeForce Overlay");

	if (!handle)
	{
		std::cout << "Failed to find NVIDIA Overlay window." << std::endl;
		return false;
	}

	auto style = GetWindowLongPtr(handle, GWL_EXSTYLE);
	style |= WS_EX_TRANSPARENT; // | WS_EX_LAYERED;
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

	*pHandle = handle;

	return true;
}

#define WINDOW_CLASS_NAME L"TopmostTransparentOverlayClass"


BOOL new_window(HWND* pHandle) {
	if (!pHandle)
	{
		std::cerr << "Error: pHandle is null." << std::endl;
		return FALSE;
	}

	WNDCLASSEX wc = {
		.cbSize = sizeof(WNDCLASSEX),
		.lpfnWndProc = DefWindowProc,
		.hInstance = nullptr,
		.hCursor = LoadCursor(nullptr, IDC_ARROW),
		.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH),
		.lpszClassName = WINDOW_CLASS_NAME,
	};

	const auto atom = RegisterClassEx(&wc);
	if (atom == 0)
	{
		std::cerr << "Failed to register window class." << std::endl;
		return FALSE;
	}


	// 2. Create the window
	HWND handle = CreateWindowEx(
		WS_EX_TOPMOST
		| WS_EX_LAYERED,
		MAKEINTATOM(atom),
		L"test",
		WS_POPUP,
		0, 0,
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
		nullptr,
		nullptr,
		nullptr,
		nullptr
	);

	if (!handle)
	{
		const auto error = GetLastError();
		std::cerr << "Failed to create window: " << error << std::endl;
		return FALSE;
	}

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

	*pHandle = handle;

	return true;
}
