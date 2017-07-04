#include "address_helpers.h"

#include <Windows.h>
#include "Psapi.h"
#include <vector>
#include <iostream>
#include "utils.h"

UINT_PTR base_offset = 0x262A9F8;
#define OFFSET_1  0x3B0
#define OFFSET_2  0x14
#define INTER_HERO_OFFSET  0xc8
extern std::ofstream myfile;
namespace
{
	HMODULE get_module(PVOID hProcess)
	{
		HMODULE h_mods[1024];
		HANDLE handle = hProcess;
		DWORD cb_needed;
		unsigned int i;

		if (EnumProcessModules(handle, h_mods, sizeof(h_mods), &cb_needed))
		{
			for (i = 0; i < (cb_needed / sizeof(HMODULE)); i++)
			{
				TCHAR sz_mod_name[MAX_PATH];
				if (GetModuleFileNameEx(handle, h_mods[i], sz_mod_name, sizeof(sz_mod_name) / sizeof(TCHAR)))
				{
					std::string str_mod_name = sz_mod_name;
					//you will need to change this to the name of the exe of the foreign process
					std::string str_mod_contain = "client.dll";
					if (str_mod_name.find(str_mod_contain) != std::string::npos)
					{
						return h_mods[i];
					}
				}
			}
		}
		return nullptr;
	}

	PVOID get_process(HWND WindowHND){
		DWORD pid;
		GetWindowThreadProcessId(WindowHND, &pid);
		return OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, 0, pid);
	}

	UINT_PTR get_process_base_address(PVOID ProcessID){
		HMODULE module = get_module(ProcessID);
		return (UINT_PTR)module;
	}

	HWND get_window(){
		return ::FindWindowEx(0, 0, 0, "Dota 2");
	}
}

std::vector<int> AddressHelper::get_selected_heroes(){
	HWND window_hnd = get_window();
	PVOID pid = get_process(window_hnd);
	UINT_PTR base_addr = get_process_base_address(pid);
	int value = 0;
	auto return_value = ReadProcessMemory(pid, (void*)base_addr, &value, sizeof(value), 0);
	auto error = GetLastError();
	std::vector<int> heroes(10);
	int errors[10];
	for (int i = 0; i < 10; ++i){
		UINT_PTR address_1 = base_addr + base_offset;
		UINT_PTR ptr_1 = 0;
		auto return_value1 = ReadProcessMemory(pid, (void*)address_1, &ptr_1, sizeof(ptr_1), 0);
		auto error_1 = GetLastError();

		UINT_PTR address_2 = ptr_1 + OFFSET_1;
		UINT_PTR ptr_2;
		ReadProcessMemory(pid, (void*)address_2, &ptr_2, sizeof(ptr_2), 0);
		auto error_2 = GetLastError();

		UINT_PTR address_3 = ptr_2 + OFFSET_2 + i*INTER_HERO_OFFSET;
		errors[i] = ReadProcessMemory(pid, (void*)address_3, &heroes[i], sizeof(heroes[i]), 0);
	}
	return heroes;
}