#pragma once

#include <map>
namespace HeroData{
	std::map<int, std::string> get_heroes();
	std::map<std::string, int> get_sorted_heroes();
}