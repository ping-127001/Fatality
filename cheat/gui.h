#pragma once
#include <d3d9.h>

namespace gui
{
	// window size
	constexpr int WIDTH = 650;
	constexpr int HEIGHT = 450;

	inline bool exit = true;

	// winapi window vars
	inline HWND window = nullptr;
	inline tagWNDCLASSEXA windowClass = { };

	// points for window movement
	inline POINTS position = { };

	// direct x state vars
	inline PDIRECT3D9 d3d = nullptr;
	inline LPDIRECT3DDEVICE9 device = nullptr;
	inline D3DPRESENT_PARAMETERS presentParameters = { };

	// handle window creations and destruction
	void createHWindow(const char* windowName, const char* className) noexcept;
	void destroyHWindow() noexcept;

	// handle device creation and destruction
	bool createDevice() noexcept;
	void resetDevice() noexcept;
	void destroyDevice() noexcept;

	// handle imgui creation and destruction
	void createImGui() noexcept;
	void destroyImGui() noexcept;

	void beginRender() noexcept;
	void endRender() noexcept;
	void Render() noexcept;
}