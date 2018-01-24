#pragma once

#include <d3d9.h>
class HUD{
public:
	static HUD* get_instance();
	void initialize(IDirect3DDevice9** device);
	void update();
private:
	void _draw();
	bool _enabled = true;
};
