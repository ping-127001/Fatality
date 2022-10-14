#include "gui.h"
#include "Handler/messageBox.h"
#include "Handler/processChecker.h"
#include "Handler/zippedChecker.h"

#include <thread>

bool debugging = false;

int __stdcall wWinMain(HINSTANCE instance, HINSTANCE previousInstance, PWSTR arguments, int commandShow)
{
	if (!debugging)
	{
		if (!processChecker::isProcessRunning("javaw"))
		{
			messageBox::createMessage("Minecraft not detected, please restart Fatality when javaw is running");
			exit(1);
		}
	}

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