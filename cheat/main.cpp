#include "gui.h"

#include <thread>

int __stdcall wWinMain(HINSTANCE instance, HINSTANCE previousInstance, PWSTR arguments, int commandShow)
{
	// create gui 
	gui::createHWindow("Fatality 1.0", "Auto Clicker");
	gui::createDevice();
	gui::createImGui();

	// render loop
	while (gui::exit)
	{
		gui::beginRender();
		gui::Render();
		gui::endRender();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	// destroy gui
	gui::destroyImGui();
	gui::destroyDevice();
	gui::destroyHWindow();

	return EXIT_SUCCESS;
}