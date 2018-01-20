#include "input_handler.h"

#include <WinUser.h>
#include "resource_manager.h"
#include "heroes.h"
#include <sstream>
#include <iomanip>

void InputHandler::handle_input(){
	if ((GetKeyState(VK_LBUTTON) & 0x100) != 0){
		_mouse_down = true;
	}
	else if (_mouse_down){
		for (auto sprite_map_entry : *ResourceManager::get_instance()->get_sprite_asset_map()){
			auto sprite_asset = sprite_map_entry.second;
			auto sprite_position = sprite_asset.info.position;
			auto sprite_size = sprite_asset.info.size;
			RECT sprite_rect;
			SetRect(&sprite_rect, sprite_position.x, sprite_position.y, sprite_position.x + sprite_size.x, sprite_position.y + sprite_size.y);
			if (_is_cursor_inside_rect(sprite_rect)){
				_on_sprite_clicked(sprite_asset.info.texture_name);
			}
		}
		_mouse_down = false;
	}

	// Enabling/disabling UI
	if ((GetKeyState(VK_F10) & 0x100) != 0){
		_f1_down = true;
	}
	else if (_f1_down){
		_f1_down = false;
		if (_ui_state_change_callback){
			_ui_state_change_callback();
		}
	}

}

void InputHandler::_on_sprite_clicked(const std::string& name){
	auto sprite_map = ResourceManager::get_instance()->get_sprite_asset_map();
	auto texture_map = ResourceManager::get_instance()->get_texture_map();
	auto text_asset_map = ResourceManager::get_instance()->get_text_asset_map();

	if (name == "left_arrow"){
		auto& hero_sprite = (*sprite_map)["hero_image"];

		if (_recommended_heroes.empty()){
			hero_sprite.texture = (*texture_map)["no_heroes"];
			return;
		}

		auto& hero_edge_text = (*text_asset_map)["hero_edge"];

		if (_hero_index && !_recommended_heroes.empty()){
			--_hero_index;
			auto hero_id = _recommended_heroes[_hero_index].first;
			auto hero_edge = _recommended_heroes[_hero_index].second;
			auto hero_name = get_heroes()[hero_id];
			hero_sprite.texture = (*texture_map)[hero_name];
			std::stringstream stream;
			stream << std::fixed << std::setprecision(2) << hero_edge;
			hero_edge_text.text = stream.str();
			if (hero_edge >= 0.0f){
				hero_edge_text.color = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
			}
			else{
				hero_edge_text.color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
			}
		}
	}
	else if (name == "right_arrow"){
		auto& hero_sprite = (*sprite_map)["hero_image"];
		if (_recommended_heroes.empty()){
			hero_sprite.texture = (*texture_map)["no_heroes"];
			return;
		}

		auto& hero_edge_text = (*text_asset_map)["hero_edge"];

		if (_hero_index != get_heroes().size() && !_recommended_heroes.empty()){
			++_hero_index;
			auto hero_id = _recommended_heroes[_hero_index].first;
			auto hero_edge = _recommended_heroes[_hero_index].second;
			auto hero_name = get_heroes()[hero_id];
			hero_sprite.texture = (*texture_map)[hero_name];
			std::stringstream stream;
			stream << std::fixed << std::setprecision(2) << hero_edge;
			hero_edge_text.text = stream.str();
			if (hero_edge >= 0.0f){
				hero_edge_text.color = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
			}
			else{
				hero_edge_text.color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
			}
		}
	}

}

bool InputHandler::_is_cursor_inside_rect(RECT rect){
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	if (cursorPos.x >= rect.left && cursorPos.x <= rect.right){
		if (cursorPos.y >= rect.top && cursorPos.y <= rect.bottom){
			return true;
		}
	}
	return false;
}
