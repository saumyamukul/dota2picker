#pragma once
#include "Psapi.h"
#include <vector>
#include <iostream>
UINT_PTR base_offset = 0x2500D40;
#define OFFSET_1  0x418
#define OFFSET_2  0x14
#define INTER_HERO_OFFSET  0xc8
extern std::ofstream myfile;

HMODULE GetModule(PVOID hProcess);


PVOID GetProcess(HWND WindowHND){
	DWORD PID;
	GetWindowThreadProcessId(WindowHND, &PID);
	return OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, 0, PID);
}

UINT_PTR GetProcessBaseAddress(PVOID ProcessID){
	HMODULE Module = GetModule(ProcessID);
	std::cout << "Module" << Module;
	int value = 0;
	auto return_value = ReadProcessMemory(ProcessID, (void*)Module, &value, sizeof(value), 0);
	auto error = GetLastError();
	return (UINT_PTR)Module;
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
				std::string strModContain = "client.dll";
				if (strModName.find(strModContain) != std::string::npos)
				{
					return hMods[i];
				}
			}
		}
	}
	return nullptr;
}

std::vector<int> GetHeroIDs(){
	HWND WindowHND = GetWindow();
	PVOID PID = GetProcess(WindowHND);
	UINT_PTR BaseAddr = GetProcessBaseAddress(PID);
	int value = 0;
	auto return_value = ReadProcessMemory(PID, (void*)BaseAddr, &value, sizeof(value), 0);
	auto error = GetLastError();
	std::vector<int> heroes(10);
	int errors[10];
	for (int i = 0; i < 10; ++i){
		UINT_PTR address_1 = BaseAddr + base_offset;
		UINT_PTR ptr_1 = 0;
		auto return_value1 = ReadProcessMemory(PID, (void*)address_1, &ptr_1, sizeof(ptr_1), 0);
		auto error_1 = GetLastError();

		UINT_PTR address_2 = ptr_1 + OFFSET_1;
		UINT_PTR ptr_2;
		ReadProcessMemory(PID, (void*)address_2, &ptr_2, sizeof(ptr_2), 0);
		auto error_2 = GetLastError();

		UINT_PTR address_3 = ptr_2 + OFFSET_2 + i*INTER_HERO_OFFSET;
		errors[i] = ReadProcessMemory(PID, (void*)address_3, &heroes[i], sizeof(heroes[i]), 0);
	}
	return heroes;
}