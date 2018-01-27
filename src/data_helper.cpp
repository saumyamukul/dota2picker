#include "data_helper.h"

#include "Psapi.h"

UINT_PTR hero_ids_base_offset = 0x2886960;
#define HERO_ID_OFFSET_1  0x3B0
#define HERO_ID_OFFSET_2  0x14
#define INTER_HERO_OFFSET  0x118

UINT_PTR radiant_gold_base_offset = 0x293B240;
UINT_PTR dire_gold_base_offset = 0x293B248;

#define RADIANT_GOLD_OFFSET_1  0x398
#define RADIANT_GOLD_OFFSET_2  0x18

HMODULE DataHelper::_get_module(HANDLE process_handle, std::string module_name)
{
	HMODULE h_mods[1024];
	DWORD cb_needed;
	unsigned int i;

	if (EnumProcessModules(process_handle, h_mods, sizeof(h_mods), &cb_needed))
	{
		for (i = 0; i < (cb_needed / sizeof(HMODULE)); i++)
		{
			TCHAR sz_mod_name[MAX_PATH];
			DWORD buffer_size = sizeof(sz_mod_name) / sizeof(TCHAR);
			if (GetModuleFileNameEx(process_handle, h_mods[i], sz_mod_name, buffer_size))
			{
				std::string str_mod_name = sz_mod_name;
				if (str_mod_name.find(module_name) != std::string::npos)
				{
					return h_mods[i];
				}
			}
		}
	}
	return nullptr;
}

UINT_PTR DataHelper::_get_module_base_address(){
	HANDLE current_process_handle = GetCurrentProcess();
	HMODULE module = _get_module(current_process_handle, "client.dll");
	return (UINT_PTR)module;
}

bool DataHelper::_is_radiant(){
	UINT_PTR base_addr = _get_module_base_address();
	if (!base_addr) return false;
	UINT_PTR radiant_gold_base_addr = base_addr + radiant_gold_base_offset;
	if (*(PUINT_PTR)radiant_gold_base_addr) return true;
	return false;
}

bool DataHelper::_is_dire(){
	UINT_PTR base_addr = _get_module_base_address();
	if (!base_addr) return false;
	UINT_PTR dire_gold_base_addr = base_addr + dire_gold_base_offset;
	if (*(PUINT_PTR)dire_gold_base_addr) return true;
	return false;
}

bool DataHelper::pick_screen_ready(){
	return _is_radiant() || _is_dire();
}

bool DataHelper::update_enemy_heroes(){
	UINT_PTR base_addr = _get_module_base_address();
	UINT_PTR hero_ids_base_address = base_addr + hero_ids_base_offset;

	UINT_PTR level_1_ptr = *(PUINT_PTR)hero_ids_base_address;
	if (!level_1_ptr) return{};

	UINT_PTR level_2_ptr = *(PUINT_PTR)(level_1_ptr + HERO_ID_OFFSET_1);
	if (!level_2_ptr) return{};

	UINT_PTR first_hero_id_ptr = (level_2_ptr + HERO_ID_OFFSET_2);
	if (!first_hero_id_ptr) return{};

	std::vector<int> heroes(5);
	int start_index = _is_radiant() ? 5 : 0;

	for (int i = 0; i < 5; ++i){
		UINT_PTR address_3 = first_hero_id_ptr + (i+start_index)*INTER_HERO_OFFSET;
		heroes[i]=(*(PUINT)address_3);
	}

	std::vector<int> enemy_ids;
	// Remove invalid ids
	for (auto enemy : heroes){
		if (enemy > 0 && enemy < 120){
			enemy_ids.push_back(enemy);
		}
	}

	// Check if values were updated
	if (enemy_ids.size() != _enemy_hero_ids.size()){
		_enemy_hero_ids = enemy_ids;
		return true;
	}
	bool changed = false;
	for (size_t i = 0; i < _enemy_hero_ids.size(); ++i){
		if (_enemy_hero_ids[i] != enemy_ids[i]){
			changed = true;
			break;
		}
	}
	if (changed){
		_enemy_hero_ids = enemy_ids;
		return true;
	}
	return false;
}