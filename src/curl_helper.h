#pragma once

#include <vector>
#include <functional>
#include <map>
#include <unordered_map>
#include "curl\curl.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include <string>
#include <atomic>

namespace CurlHelper{
	std::atomic<bool> inited = false;

	size_t writer(char *data, size_t size, size_t nmemb,
		std::string *writerData)
	{
		if (writerData == NULL)
			return 0;

		writerData->append(data, size*nmemb);

		return size * nmemb;
	}

	bool init(CURL *&conn, const char *url, char* error_buffer, std::string& buffer)
	{
		CURLcode code;

		conn = curl_easy_init();

		if (conn == NULL) {
			fprintf(stderr, "Failed to create CURL connection\n");
			exit(EXIT_FAILURE);
		}

		code = curl_easy_setopt(conn, CURLOPT_SSL_VERIFYPEER, 0);
		if (code != CURLE_OK) {
			fprintf(stderr, "Failed to set verify peer [%d]\n", code);
			return false;
		}

		code = curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, error_buffer);
		if (code != CURLE_OK) {
			fprintf(stderr, "Failed to set error buffer [%d]\n", code);
			return false;
		}

		code = curl_easy_setopt(conn, CURLOPT_URL, url);
		if (code != CURLE_OK) {
			fprintf(stderr, "Failed to set URL [%s]\n", error_buffer);
			return false;
		}

		code = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1L);
		if (code != CURLE_OK) {
			fprintf(stderr, "Failed to set redirect option [%s]\n", error_buffer);
			return false;
		}

		code = curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, writer);
		if (code != CURLE_OK) {
			fprintf(stderr, "Failed to set writer [%s]\n", error_buffer);
			return false;
		}

		code = curl_easy_setopt(conn, CURLOPT_WRITEDATA, &buffer);
		if (code != CURLE_OK) {
			fprintf(stderr, "Failed to set write data [%s]\n", error_buffer);
			return false;
		}

		return true;
	}

	void query_hero_win_rates(std::vector<int> enemy_hero_ids, std::function<void(std::vector<std::pair<int, float>>)> callback){
		std::map<float, int> sorted_map;
		if (enemy_hero_ids.empty()){
			return;
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
		for (auto enemy_id : enemy_hero_ids){
			if (!ids_string.empty()){
				ids_string += ",";
			}
			ids_string += std::to_string(enemy_id);
		}
		auto url = std::string("https://dotaedge.com/counters/") + ids_string + std::string("?minMmr=1000&maxMmr=9999&timespan=last_month");
		std::string buffer;
		buffer.resize(50000);
		char error_buffer[CURL_ERROR_SIZE];

		if (!init(conn, url.c_str(), error_buffer, buffer)) {
			fprintf(stderr, "Connection initializion failed\n");
			exit(EXIT_FAILURE);
		}

		//// Retrieve content for the URL
		buffer.clear();
		code = curl_easy_perform(conn);
		curl_easy_cleanup(conn);

		if (code != CURLE_OK) {
			return;
		}

		//// 1. Parse a JSON string into DOM.
		const char* json = buffer.c_str();
		rapidjson::Document d;
		d.Parse(json);

		//// 2. Modify it by DOM.
		rapidjson::Value& s = d["enemies"];
		auto type = s.GetType();
		assert(s.IsArray());
		for (rapidjson::SizeType i = 0; i < enemy_hero_ids.size(); i++) {// Uses SizeType instead of size_t
			auto enemy = s[i].GetObject();
			auto enemyID = enemy["enemy"].GetInt();
			auto counters = enemy["counters"].GetArray();
			for (rapidjson::SizeType j = 0; j < counters.Size(); j++) {// Uses SizeType instead of size_t
				auto counter = counters[j].GetObject();
				auto counterID = counter["id"].GetInt();
				auto edge = counter["edge"].GetFloat();
				if (edge_map.find(counterID) != edge_map.end()){
					edge_map[counterID] += edge *100.0f / enemy_hero_ids.size();
				}
				else{
					edge_map[counterID] = edge*100.0f / enemy_hero_ids.size();
				}
				auto matchup = counter["matchup"].GetString();
			}
		}

		for (auto iterator = edge_map.begin(); iterator != edge_map.end(); ++iterator){
			sorted_map.insert({ iterator->second, iterator->first });
		}

		std::vector<std::pair<int, float>> heroes;
		for (auto iterator = sorted_map.rbegin(); iterator != sorted_map.rend(); ++iterator){
			heroes.push_back({ iterator->second, iterator->first });
		}
		callback(heroes);
	}
}