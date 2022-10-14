#include "messageBox.h"

#include <Windows.h>
#include <iostream>

void messageBox::createMessage(LPCSTR message)
{
    MessageBox(FindWindowA("ConsoleWindowClass", NULL), message, "Fatality", MB_OK);
}