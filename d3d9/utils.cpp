#include "utils.h"
#include <WinUser.h>
#include "resource_manager.h"

bool mouse_down = false;
namespace{

	HWND get_window(){
		return ::FindWindowEx(0, 0, 0, "Dota 2");
	}

	bool is_inside_rect(RECT rect){
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		if (cursorPos.x >= rect.left && cursorPos.x <= rect.right){
			if (cursorPos.y >= rect.top && cursorPos.y <= rect.bottom){
				return true;
			}
		}
		return false;
	}
}

RECT Utils::get_window_rect(){
	auto window_handle = get_window();
	RECT window_rect;
	GetWindowRect(window_handle, &window_rect);
	return window_rect;
}

void Utils::handle_input(){
	if ((GetKeyState(VK_LBUTTON) & 0x100) != 0){
		mouse_down = true;
	}
	else if (mouse_down){
		for (auto sprite : *ResourceManager::get_sprites()){
			RECT sprite_rect;
			SetRect(&sprite_rect, sprite.position.x, sprite.position.y, sprite.position.x + sprite.size.x, sprite.position.y + sprite.size.y);
			if (is_inside_rect(sprite_rect) && sprite.callback){
				sprite.callback();
			}
		}
		mouse_down = false;
	}
}
