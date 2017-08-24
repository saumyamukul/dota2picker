#include "address_helpers.h"

#include <Windows.h>
#include "Psapi.h"
#include <vector>
#include <iostream>
#include "utils.h"
#include <map>
#include <unordered_map>
#include <string>
#include <fstream>
#include "Includes\rapidjson\document.h"
#include "heroes.h"
#include "Includes\curl\curl.h"
#include "Includes/libxml/HTMLparser.h"
#include <iostream>
#include "Includes/rapidjson/document.h"
#include "Includes/rapidjson/writer.h"

static char errorBuffer[CURL_ERROR_SIZE];
static std::string buffer;

UINT_PTR base_offset = 0x2680010;
#define OFFSET_1  0x3B0
#define OFFSET_2  0x14
#define INTER_HERO_OFFSET  0xd8

UINT_PTR radiant_gold_base_offset = 0x2735500;
#define RADIANT_GOLD_OFFSET_1  0x398
#define RADIANT_GOLD_OFFSET_2  0x18

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
		PVOID handle =  OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, 0, pid);
		return handle;
	}

	UINT_PTR get_process_base_address(PVOID ProcessID){
		HMODULE module = get_module(ProcessID);
		return (UINT_PTR)module;
	}

	HWND get_window(){
		return ::FindWindowEx(0, 0, 0, "Dota 2");
	}

	bool is_radiant(){
		HWND window_hnd = get_window();
		PVOID pid = get_process(window_hnd);
		UINT_PTR base_addr = get_process_base_address(pid);
		UINT_PTR radiant_gold_base_addr = base_addr + radiant_gold_base_offset;
		UINT_PTR ptr_1 = 123;
		auto error1 = GetLastError();
		auto return_value = ReadProcessMemory(pid, (void*)radiant_gold_base_addr, &ptr_1, sizeof(ptr_1), 0);
		auto error = GetLastError();
		return ptr_1;
	}

	//std::map<float, int>  get_hero_win_rates(std::vector<int> enemy_heroes){
	//	std::ifstream t("Assets/win_rates.txt");
	//	std::string str;

	//	t.seekg(0, std::ios::end);
	//	str.reserve(t.tellg());
	//	t.seekg(0, std::ios::beg);

	//	str.assign((std::istreambuf_iterator<char>(t)),
	//		std::istreambuf_iterator<char>());

	//	const char* json = str.c_str();
	//	rapidjson::Document d;
	//	d.Parse(json);

	//	std::unordered_map<int, float> edge_map;
	//	rapidjson::Value& s = d["win_rates"];
	//	auto type = s.GetType();
	//	assert(s.IsArray());
	//	for (rapidjson::SizeType i = 0; i < enemy_heroes.size(); i++) {// Uses SizeType instead of size_t
	//		int enemy_index = 0;
	//		int enemy_id = enemy_heroes[i];
	//		for (auto enemy : get_sorted_heroes()){
	//			if (enemy.second != enemy_id){
	//				++enemy_index;
	//			}
	//		}
	//		auto herodata = s[enemy_index].GetArray();
	//		auto sorted_heroes = get_sorted_heroes();
	//		auto iterator = sorted_heroes.begin();
	//		int hero_index = 0;
	//		auto size = herodata.Size();
	//		for (rapidjson::SizeType j = 0; j < herodata.Size(); j++) {// Uses SizeType instead of size_t
	//			auto hero_id = iterator->second;
	//			++iterator;
	//			bool hero_already_picked = false;
	//			for (auto hero_id : enemy_heroes){
	//				if (hero_id == hero_id){
	//					hero_already_picked = true;
	//					break;
	//				}
	//			}
	//			if (hero_already_picked) continue;
	//			auto counterdata = herodata[j].GetArray();
	//			auto value = counterdata[0].GetString();
	//			float val = std::stof(value);
	//			if (edge_map.find(j) != edge_map.end()){
	//				edge_map[hero_id] += val;
	//			}
	//			else{
	//				edge_map[hero_id] = val;
	//			}
	//		}
	//	}
	//	std::map<float, int> sorted_map;
	//	for (auto iterator = edge_map.begin(); iterator != edge_map.end(); ++iterator){
	//		sorted_map.insert({ iterator->second, iterator->first });
	//	}
	//	return sorted_map;
	//}

	int writer(char *data, size_t size, size_t nmemb,
		std::string *writerData)
	{
		if (writerData == NULL)
			return 0;

		writerData->append(data, size*nmemb);

		return size * nmemb;
	}

	bool init(CURL *&conn, char *url)
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
		std::unordered_map<int, float> edge_map;

		CURL *conn = NULL;
		CURLcode code;
		std::string title;


		if (!inited){
			curl_global_init(CURL_GLOBAL_DEFAULT);
			inited = true;
		}
		//// Initialize CURL connection
		char* url = "https://dotaedge.com/counters/1,69,59,67,64?minMmr=1000&maxMmr=9999&timespan=last_month";
		if (!init(conn, url)) {
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
		for (rapidjson::SizeType i = 0; i < 5; i++) {// Uses SizeType instead of size_t
			auto enemy = s[i].GetObject();
			auto enemyID = enemy["enemy"].GetInt();
			auto counters = enemy["counters"].GetArray();
			for (rapidjson::SizeType j = 0; j < counters.Size(); j++) {// Uses SizeType instead of size_t
				auto counter = counters[j].GetObject();
				auto counterID = counter["id"].GetInt();
				auto edge = counter["edge"].GetFloat();
				if (edge_map.find(counterID) != edge_map.end()){
					edge_map[counterID] += edge;
				}
				else{
					edge_map[counterID] = edge;
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
	HWND window_hnd = get_window();
	PVOID pid = get_process(window_hnd);
	UINT_PTR base_addr = get_process_base_address(pid);

	UINT_PTR address_1 = base_addr + base_offset;
	UINT_PTR ptr_1 = 0;
	ReadProcessMemory(pid, (void*)address_1, &ptr_1, sizeof(ptr_1), 0);
	auto error_1 = GetLastError();
	if (!address_1) return{}; 

	UINT_PTR address_2 = ptr_1 + OFFSET_1;
	UINT_PTR ptr_2;
	ReadProcessMemory(pid, (void*)address_2, &ptr_2, sizeof(ptr_2), 0);
	auto error_2 = GetLastError();

	std::vector<int> heroes(5);
	int errors[10];
	int start_index = is_radiant() ? 0 : 5;
	int end_index = is_radiant() ? 5 : 10;
	int vec_index = 0;

	for (int i = start_index; i < end_index; ++i){
		UINT_PTR address_3 = ptr_2 + OFFSET_2 + i*INTER_HERO_OFFSET;
		errors[i] = ReadProcessMemory(pid, (void*)address_3, &heroes[vec_index], sizeof(heroes[vec_index]), 0);
		++vec_index;
	}
	return heroes;
}

std::vector<int> AddressHelper::get_recommended_hero_list(){
	auto enemy_heroes = get_enemy_heroes();
	if (enemy_heroes.empty()) return{};
	auto rates = get_hero_win_rates(enemy_heroes);
	std::vector<int> heroes;
	for (auto hero : heroes){
		heroes.push_back(0);
	}
	return heroes;
}