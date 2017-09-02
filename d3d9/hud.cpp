#include "hud.h"

#include "resource_manager.h"
#include "input_handler.h"

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
	if (!_enabled) return;
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