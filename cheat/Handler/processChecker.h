#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>

#pragma once
namespace processChecker
{
    bool isProcessRunning(const TCHAR* const executableName)
    {
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(PROCESSENTRY32);

        const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

        if (!Process32First(snapshot, &entry))
        {
            CloseHandle(snapshot);
            return false;
        }

        do
        {
            if (!_tcsicmp(entry.szExeFile, executableName))
            {
                CloseHandle(snapshot);
                return true;
            }
        } while (Process32Next(snapshot, &entry));

        CloseHandle(snapshot);
        return false;
    }
}