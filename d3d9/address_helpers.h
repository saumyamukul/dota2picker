#pragma once
#include "Psapi.h"
extern std::ofstream myfile;

HMODULE GetModule(PVOID hProcess);

DWORD GetProcessBaseAddress(){
	HWND WindowHandle = ::FindWindowEx(0, 0, 0, "Dota 2");
	std::cout << "Win handle" << (void*)WindowHandle;
	DWORD PID;
	GetWindowThreadProcessId(WindowHandle, &PID);
	std::cout << "PID" << PID;
	PVOID hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, 0, PID);
	HMODULE Module = GetModule(hProcess);
	std::cout << "Module" << Module;
	int value = 0;
	auto return_value = ReadProcessMemory(hProcess, (void*)Module, &value, sizeof(value), 0);
	auto error = GetLastError();
	return (DWORD)Module;
}

HWND GetWindow(){
	return ::FindWindowEx(0, 0, 0, "Dota 2");
}

HMODULE GetModule(PVOID hProcess)
{
	HMODULE hMods[1024];
	HANDLE pHandle = hProcess;
	DWORD cbNeeded;
	unsigned int i;

	if (EnumProcessModules(pHandle, hMods, sizeof(hMods), &cbNeeded))
	{
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];
			if (GetModuleFileNameEx(pHandle, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
			{
				std::string strModName = szModName;
				//you will need to change this to the name of the exe of the foreign process
				std::string strModContain = "dota2.exe";
				if (strModName.find(strModContain) != std::string::npos)
				{
					return hMods[i];
				}
			}
		}
	}
	return nullptr;
}