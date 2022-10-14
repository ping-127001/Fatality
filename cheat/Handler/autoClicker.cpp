#include "autoClicker.h"

#include <iostream>
#include <Windows.h>

bool clickerstate = true;

void autoClicker::Click()
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