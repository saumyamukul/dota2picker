#pragma once

#include <vector>
#include <functional>
#include <windows.h>

class InputHandler{
public:
	static InputHandler* get_instance(){
		static InputHandler instance;
		return &instance;
	}
	RECT get_window_rect();
	void handle_input();
	void on_ui_state_changed(std::function<void()> callback){ _ui_state_change_callback = callback; }
	void set_recommended_heroes(std::vector<std::pair<int, float>> heroes){ _recommended_heroes = heroes; }
private:
	void _on_sprite_clicked(const std::string& name);
	bool _is_cursor_inside_rect(RECT rect);
	std::vector<std::pair<int, float>> _recommended_heroes;
	std::function<void()> _ui_state_change_callback = nullptr;
	int _hero_index = 0;
	bool _mouse_down = false;
	bool _f1_down = false;
	bool _ui_enabled = true;
};