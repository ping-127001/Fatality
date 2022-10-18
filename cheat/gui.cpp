#include "gui.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui//imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT, WPARAM wideParameter, LPARAM longParameter);

long __stdcall WindowProcess(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
	{
		return true;
	}

	switch (message)
	{
		case WM_SIZE: 
		{
			if ((wideParameter & 0xffff0) == SC_KEYMENU) // disable ALT application menu
			{
				return 0;
			}
		}break;

		case WM_DESTROY: 
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_LBUTTONDOWN: 
		{
			gui::position = MAKEPOINTS(longParameter); // set click points
		}return 0;

		case WM_MOUSEMOVE: 
		{
			if (wideParameter == MK_LBUTTON)
			{
				const auto points = MAKEPOINTS(longParameter);
				auto rect = ::RECT{ };

				GetWindowRect(gui::window, &rect);

				rect.left += points.x - gui::position.x;
				rect.top += points.y - gui::position.y;

				if (gui::position.x >= 0 && gui::position.x <= gui::WIDTH && gui::position.y >= 0 && gui::position.y <= 19)
				{
					SetWindowPos(gui::window, HWND_TOPMOST, rect.left, rect.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER);
				}
			}
		}
}

	return DefWindowProcW(window, message, wideParameter, longParameter);
}

void gui::createHWindow(const char* windowName, const char* className) noexcept
{
	windowClass.cbSize = sizeof(WNDCLASSEXA);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = WindowProcess;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = className;
	windowClass.hIconSm = 0;

	RegisterClassExA(&windowClass);

	window = CreateWindowA
	(
		className,
		windowName,
		WS_POPUP,
		100,
		100,
		WIDTH,
		HEIGHT,
		0,
		0,
		windowClass.hInstance,
		0
	);

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);
}

void gui::destroyHWindow() noexcept
{
	DestroyWindow(window);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool gui::createDevice() noexcept
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d)
	{
		return false;
	}

	ZeroMemory(&presentParameters, sizeof(presentParameters));

	presentParameters.Windowed = TRUE;
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
	presentParameters.EnableAutoDepthStencil = TRUE;
	presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (d3d->CreateDevice
	(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		window,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&presentParameters,
		&device) < 0)
		return false;

	return true;
}


void gui::resetDevice() noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = device->Reset(&presentParameters);

	if (result == D3DERR_INVALIDCALL)
	{
		IM_ASSERT(0);
	}

	ImGui_ImplDX9_CreateDeviceObjects();
}


void gui::destroyDevice() noexcept
{
	if (device)
	{
		device->Release();
		device = nullptr;
	}

	if (d3d)
	{
		d3d->Release();
		d3d = nullptr;
	}

}


void gui::createImGui() noexcept
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ::ImGui::GetIO();

	io.IniFilename = NULL;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}

void gui::destroyImGui() noexcept
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void gui::beginRender() noexcept
{
	MSG message;
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	// start imgui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void gui::endRender() noexcept
{
	ImGui::EndFrame();

	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	// clear buffer
	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	if (device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);

	if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		resetDevice();
	}
}

void gui::Render() noexcept
{
	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
	ImGui::Begin
	(
		"Fatality",
		&exit,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoMove
	);

	// controls

	static bool autoclickertoggle = false;
	if (ImGui::Checkbox("Auto Clicker", &autoclickertoggle))
	{
		//autoClick(); //auto clicker needs work
	}

	static int cpsint = 0;
	ImGui::SliderInt("cps", &cpsint, 1, 25);
	

	// styling
	ImVec4* colors = ImGui::GetStyle().Colors;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(18, 15, 39, 255));
	ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(234, 15, 87, 255));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(234, 15, 87, 255));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(234, 15, 87, 255));
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, IM_COL32(234, 15, 87, 255));
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, IM_COL32(234, 15, 87, 255));
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, IM_COL32(234, 15, 87, 255));
	ImGui::PushStyleColor(ImGuiCol_SliderGrab, IM_COL32(234, 15, 87, 255));
	ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, IM_COL32(234, 15, 87, 255));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(27, 23, 74, 255));
	ImGui::PushStyleColor(ImGuiCol_CheckMark, IM_COL32(234, 15, 87, 255));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(27, 23, 74, 255));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(27, 23, 74, 255));
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, IM_COL32(0, 0, 0, 255));
	colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f); // transparent border

	// debugging stuff
	//ImGui::ShowStyleEditor();

	ImGui::End();
}

bool clickerstate = true;

void autoClick()
{
	while (true)
	{
		if (clickerstate)
		{
			// mouse click
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		}
		Sleep(10); // this determines the auto clicker speed
	}
}