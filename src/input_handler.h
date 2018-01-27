#pragma once

#include <functional>
#include <windows.h>

class InputHandler{
public:
	static InputHandler* get_instance(){
		static InputHandler instance;
		return &instance;
	}
	void handle_input();
	void on_ui_state_changed(std::function<void()> callback){ _ui_state_change_callback = callback; }
private:
	void _on_sprite_clicked(const std::string& name);
	bool _is_cursor_inside_rect(RECT rect);
	std::function<void()> _ui_state_change_callback = nullptr;
	bool _mouse_down = false;
	bool _f1_down = false;
	bool _ui_enabled = true;
};