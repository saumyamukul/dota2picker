#include "address_helpers.h"

#include <Windows.h>
#include "Psapi.h"
#include <vector>
#include <iostream>
#include "input_handler.h"
#include <map>
#include <unordered_map>
#include <string>
#include <fstream>
#include "rapidjson\document.h"
#include "heroes.h"
#include "curl\curl.h"
#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"

static char errorBuffer[CURL_ERROR_SIZE];
static std::string buffer;

UINT_PTR hero_ids_base_offset = 0x2886930;
#define HERO_ID_OFFSET_1  0x3B0
#define HERO_ID_OFFSET_2  0x14
#define INTER_HERO_OFFSET  0x118

UINT_PTR radiant_gold_base_offset = 0x293B210;
UINT_PTR dire_gold_base_offset = 0x293B218;

#define RADIANT_GOLD_OFFSET_1  0x398
#define RADIANT_GOLD_OFFSET_2  0x18

extern std::ofstream myfile;
namespace
{
	HMODULE get_module(HANDLE process_handle, std::string module_name)
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

	UINT_PTR get_module_base_address(){
		HANDLE current_process_handle = GetCurrentProcess();
		HMODULE module = get_module(current_process_handle, "client.dll");
		return (UINT_PTR)module;
	}

	bool is_radiant(){
		UINT_PTR base_addr = get_module_base_address();
		UINT_PTR radiant_gold_base_addr = base_addr + radiant_gold_base_offset;
		if (radiant_gold_base_addr) return true;
		return false;
	}

	bool is_dire(){
		UINT_PTR base_addr = get_module_base_address();
		UINT_PTR dire_gold_base_addr = base_addr + dire_gold_base_offset;
		if (dire_gold_base_addr) return true;
		return false;
	}

	bool pick_screen_ready(){
		return is_radiant() || is_dire();
	}

	size_t writer(char *data, size_t size, size_t nmemb,
		std::string *writerData)
	{
		if (writerData == NULL)
			return 0;

		writerData->append(data, size*nmemb);

		return size * nmemb;
	}

	bool init(CURL *&conn, const char *url)
	{
		CURLcode code;

		conn = curl_easy_init();

		if (conn == NULL) {
			fprintf(stderr, "Failed to create CURL connection\n");
			exit(EXIT_FAILURE);
		}

		code = curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, errorBuffer);
		if (code != CURLE_OK) {
			fprintf(stderr, "Failed to set error buffer [%d]\n", code);
			return false;
		}

		code = curl_easy_setopt(conn, CURLOPT_URL, url);
		if (code != CURLE_OK) {
			fprintf(stderr, "Failed to set URL [%s]\n", errorBuffer);
			return false;
		}

		code = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1L);
		if (code != CURLE_OK) {
			fprintf(stderr, "Failed to set redirect option [%s]\n", errorBuffer);
			return false;
		}

		code = curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, writer);
		if (code != CURLE_OK) {
			fprintf(stderr, "Failed to set writer [%s]\n", errorBuffer);
			return false;
		}

		code = curl_easy_setopt(conn, CURLOPT_WRITEDATA, &buffer);
		if (code != CURLE_OK) {
			fprintf(stderr, "Failed to set write data [%s]\n", errorBuffer);
			return false;
		}

		return true;
	}

	bool inited = false;
	std::map<float, int>  get_hero_win_rates(std::vector<int> enemy_heroes){
		std::map<float, int> sorted_map;
		if (enemy_heroes.empty()){
			return sorted_map;
		}
		std::unordered_map<int, float> edge_map;

		CURL *conn = NULL;
		CURLcode code;
		std::string title;


		if (!inited){
			curl_global_init(CURL_GLOBAL_DEFAULT);
			inited = true;
		}
		//// Initialize CURL connection
		std::string ids_string;
		for (auto enemy_id : enemy_heroes){
			if (!ids_string.empty()){
				ids_string += ",";
			}
			ids_string += std::to_string(enemy_id);
		}
		auto url = std::string("https://dotaedge.com/edge/") + ids_string + std::string("?minMmr=1000&maxMmr=9999&timespan=last_month");
		if (!init(conn, url.c_str())) {
			fprintf(stderr, "Connection initializion failed\n");
			exit(EXIT_FAILURE);
		}

		//// Retrieve content for the URL
		buffer.clear();
		code = curl_easy_perform(conn);
		curl_easy_cleanup(conn);

		if (code != CURLE_OK) {
			return sorted_map;
		}

		//// 1. Parse a JSON string into DOM.
		const char* json = buffer.c_str();
		rapidjson::Document d;
		d.Parse(json);

		//// 2. Modify it by DOM.
		rapidjson::Value& s = d["enemies"];
		auto type = s.GetType();
		assert(s.IsArray());
		for (rapidjson::SizeType i = 0; i < enemy_heroes.size(); i++) {// Uses SizeType instead of size_t
			auto enemy = s[i].GetObject();
			auto enemyID = enemy["enemy"].GetInt();
			auto counters = enemy["counters"].GetArray();
			for (rapidjson::SizeType j = 0; j < counters.Size(); j++) {// Uses SizeType instead of size_t
				auto counter = counters[j].GetObject();
				auto counterID = counter["id"].GetInt();
				auto edge = counter["edge"].GetFloat();
				if (edge_map.find(counterID) != edge_map.end()){
					edge_map[counterID] += edge *100.0f/enemy_heroes.size();
				}
				else{
					edge_map[counterID] = edge*100.0f / enemy_heroes.size();
				}
				auto matchup = counter["matchup"].GetString();
			}
		}

		for (auto iterator = edge_map.begin(); iterator != edge_map.end(); ++iterator){
			sorted_map.insert({ iterator->second, iterator->first });
		}

		return sorted_map;
	}
}

std::vector<int> AddressHelper::get_enemy_heroes(){
	if (!pick_screen_ready()) return{};

	UINT_PTR base_addr = get_module_base_address();
	UINT_PTR hero_ids_base_address = base_addr + hero_ids_base_offset;

	UINT_PTR level_1_ptr = *(PUINT_PTR)hero_ids_base_address;
	if (!level_1_ptr) return{};

	UINT_PTR level_2_ptr = *(PUINT_PTR)(level_1_ptr + HERO_ID_OFFSET_1);
	if (!level_2_ptr) return{};

	UINT_PTR first_hero_id_ptr = (level_2_ptr + HERO_ID_OFFSET_2);
	if (!first_hero_id_ptr) return{};

	std::vector<int> heroes(5);
	int start_index = is_radiant() ? 5 : 0;
	int end_index = is_radiant() ? 10 : 5;
	int vec_index = 0;

	for (int i = start_index; i < end_index; ++i){
		UINT_PTR address_3 = first_hero_id_ptr + i*INTER_HERO_OFFSET;
		heroes.push_back(*(PUINT)address_3);
		++vec_index;
	}

	std::vector<int> enemies;
	// Remove invalid ids
	for (auto enemy : heroes){
		if (enemy > 0 && enemy < 120){
			enemies.push_back(enemy);
		}
	}
	return enemies;
}

std::vector<std::pair<int,float>> AddressHelper::get_recommended_hero_list(){
	auto enemy_heroes = get_enemy_heroes();
	if (enemy_heroes.empty()) return{};
	auto rates = get_hero_win_rates(enemy_heroes);
	std::vector<std::pair<int,float>> heroes;
	for (auto iterator = rates.rbegin(); iterator != rates.rend(); ++iterator){
		heroes.push_back({ iterator->second, iterator->first });
	}
	return heroes;
}