#pragma once

#include <atomic>
#include <d3d9.h>

class HUD{
public:
	static HUD* get_instance();
	void initialize(IDirect3DDevice9** device);
	void update();
private:
	void _draw();

	void _update_hero_recommendations();
	bool _enabled = true;
	int _update_counter = 0;
	std::atomic<bool> recommendations_updated = false;
};
