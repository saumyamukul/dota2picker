#pragma once

#include <vector>

namespace AddressHelper{
	std::vector<int> get_enemy_heroes();
    std::vector<std::pair<int, float>>  get_recommended_hero_list();
}