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
			SetRect(&sprite_rect, (int)sprite_position.x, (int)sprite_position.y, (int)(sprite_position.x + sprite_size.x), (int)(sprite_position.y + sprite_size.y));
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
	if (name == "left_arrow"){
		ResourceManager::get_instance()->goto_previous_hero();
	}
	else if (name == "right_arrow"){
		ResourceManager::get_instance()->goto_next_hero();
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
