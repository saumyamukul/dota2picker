#pragma once
#include "Psapi.h"
extern std::ofstream myfile;

HMODULE GetModule(PVOID hProcess);

DWORD GetProcessBaseAddress(){
	HWND WindowHandle = FindWindow(nullptr, "dota2.exe");
	myfile << "Win handle" << WindowHandle;
	DWORD PID;
	GetWindowThreadProcessId(WindowHandle, &PID);
	myfile << "PID" << PID;
	PVOID hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, 0, PID);
	HMODULE Module = GetModule(hProcess);
	myfile << "Module" << Module;
	return (DWORD)Module;
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
					CloseHandle(pHandle);
					return hMods[i];
				}
			}
		}
	}
	return nullptr;
}