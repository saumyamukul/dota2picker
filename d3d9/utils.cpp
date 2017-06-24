#include "utils.h"
#include <windows.h>
#include <WinUser.h>
#include "texture_loader.h"

bool mouse_down = false;
namespace{
	bool InsideRect(RECT rect){
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

void Utils::handle_input(){
	if ((GetKeyState(VK_LBUTTON) & 0x100) != 0){
		mouse_down = true;
	}
	else if (mouse_down){
		for (auto sprite : *TextureLoader::get_sprites()){
			RECT sprite_rect;
			SetRect(&sprite_rect, sprite.position.x, sprite.position.y, sprite.position.x + sprite.size.x, sprite.position.y + sprite.size.y);
			if (InsideRect(sprite_rect) && sprite.callback){
				sprite.callback();
			}
		}
		mouse_down = false;
	}
}
