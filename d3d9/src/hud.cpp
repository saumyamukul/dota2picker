#include "hud.h"

#include "data_helper.h"
#include "curl_helper.h"
#include "input_handler.h"
#include "resource_manager.h"
#include <thread>

HUD* HUD::get_instance(){
	static HUD instance;
	return &instance;
}

void HUD::initialize(IDirect3DDevice9** device){
	ResourceManager::get_instance()->initialize(device);
	InputHandler::get_instance()->on_ui_state_changed([this]{
		_enabled = !_enabled;
	});
}

void HUD::update(){
	InputHandler::get_instance()->handle_input();
	if (recommendations_updated){
		recommendations_updated = false;
		ResourceManager::get_instance()->update();
	}

	if (!_enabled) return;

	if (DataHelper::get_instance()->pick_screen_ready()){
		// Update heroes every 180 frames
		++_update_counter;
		if (_update_counter >= 180){
			_update_hero_recommendations();
		}
	}
	else{
		ResourceManager::get_instance()->reset_sprites();
	}
	

	// Draw sprites
	D3DCOLOR color = D3DCOLOR_ARGB(255, 255, 255, 255);
	for (auto map_entry : *ResourceManager::get_instance()->get_sprite_asset_map()){
		auto sprite_asset = map_entry.second;
		if (!sprite_asset.texture) continue;
		auto d3d_sprite = sprite_asset.sprite;
		D3DXVECTOR3 position = D3DXVECTOR3(sprite_asset.info.position.x, sprite_asset.info.position.y, 0.0f);
		d3d_sprite->Begin(D3DXSPRITE_ALPHABLEND);
		d3d_sprite->Draw(sprite_asset.texture, NULL, NULL, &position, color);
		d3d_sprite->End();
	}

	// Draw text
	auto font = ResourceManager::get_instance()->get_default_font();
	for (auto map_entry : *ResourceManager::get_instance()->get_text_asset_map()){
		auto text_asset = map_entry.second;
		font->DrawText(NULL, text_asset.text.c_str(), -1, &text_asset.rect, DT_CENTER | DT_NOCLIP, text_asset.color);
	}
}

void HUD::_update_hero_recommendations(){
	if (!DataHelper::get_instance()->update_enemy_heroes()) return;

	auto enemy_hero_ids = DataHelper::get_instance()->get_enemy_hero_ids();
	if (enemy_hero_ids.empty()) return;

	// Query website for hero win rates on a separate thread
	auto callback = [this](std::vector<std::pair<int, float>> hero_map){
		// Access to the recommended hero map is thread safe
		ResourceManager::get_instance()->set_recommended_hero_map(hero_map);
		recommendations_updated = true;
	};
	std::thread query_thread(&CurlHelper::query_hero_win_rates, enemy_hero_ids, callback);
	query_thread.detach();
}