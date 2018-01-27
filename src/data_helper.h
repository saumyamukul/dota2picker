#pragma once

#include <vector>
#include <Windows.h>

class DataHelper{
public:
	static DataHelper* get_instance(){
		static DataHelper instance;
		return &instance;
	}
	bool update_enemy_heroes();
	std::vector<int> get_enemy_hero_ids(){ return _enemy_hero_ids; }
	bool pick_screen_ready();

private:

	HMODULE _get_module(HANDLE process_handle, std::string module_name);
	UINT_PTR _get_module_base_address();
	bool _is_radiant();
	bool _is_dire();
	std::vector<int> _enemy_hero_ids;
};