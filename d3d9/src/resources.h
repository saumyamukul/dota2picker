#pragma once

#include <vector>
#include "asset_definitions.h"
#include "input_handler.h"

class Resources{
public:
	Resources(){
		auto window_rect = _get_window_rect();
		auto window_width = window_rect.right - window_rect.left;
		auto window_height = window_rect.bottom - window_rect.top;

		_image_width = window_width*_image_width_percent;
		_image_height = window_height*_image_height_percent;

		_dock_width = window_width*_dock_width_percent;
		_dock_height = window_height*_dock_height_percent;

		_arrow_width = window_width*_arrow_width_percent;
		_arrow_height = window_height*_arrow_height_percent;

		_text_width = window_width*_text_width_percent;
		_text_height = window_width*_text_height_percent;

		_dock_padding_top = (window_height - _dock_height) / 2.0f;
		_image_padding_left = (_dock_width - _image_width) / 2.0f;
		_image_padding_top = (_dock_height - _image_height) / 2.0f;
		_text_padding_top = (int)(window_height*1.8f);

		_dock_position = D3DXVECTOR2(window_width - _dock_width, _dock_padding_top);
		_image_position = D3DXVECTOR2(_dock_position.x + _image_padding_left, _dock_position.y + _image_padding_top);
		_arrow_center_position = D3DXVECTOR2(window_width - _dock_width / 2.0f, _image_position.y + _image_height / 2.0f - _arrow_height / 2.0f);
		_text_center_position = D3DXVECTOR2(window_width - _dock_width / 2.0f, _image_position.y + _image_height + _text_padding_top);
	}

	std::vector<TextureInfo> get_hud_texture_info(){
		std::vector<TextureInfo> texture_info = {
			{ "dock_frame", D3DXVECTOR2(_dock_width, _dock_height) },
			{ "left_arrow", D3DXVECTOR2(_arrow_width, _arrow_height) },
			{ "right_arrow", D3DXVECTOR2(_arrow_width, _arrow_height) },
			{ "default_image", D3DXVECTOR2(_image_width, _image_height) },
		};
		return texture_info;
	}

	std::vector<SpriteInfo> get_sprite_info(){
		auto arrow_offset_from_center = _image_width / 1.9f;
		D3DXVECTOR2 left_arrow_pos = _arrow_center_position + D3DXVECTOR2(-arrow_offset_from_center - _arrow_width / 2.0f, 0.0f);
		D3DXVECTOR2 right_arrow_pos = _arrow_center_position + D3DXVECTOR2(arrow_offset_from_center - _arrow_width / 2.0f, 0.0f);

		std::vector<SpriteInfo> sprite_info = {
			{ "dock_frame", "dock_frame", _dock_position, D3DXVECTOR2(_dock_width, _dock_height) },
			{ "hero_image", "default_image", _image_position, D3DXVECTOR2(_image_width, _image_height) },
			{ "left_arrow", "left_arrow", left_arrow_pos, D3DXVECTOR2(_arrow_width, _arrow_height) },
			{ "right_arrow", "right_arrow", right_arrow_pos, D3DXVECTOR2(_arrow_width, _arrow_height) },
		};
		return sprite_info;
	}

	std::vector<TextAsset> get_text_assets(){
		RECT font_rect;
		SetRect(&font_rect, (int)_text_center_position.x - _text_width / 2, (int)_text_center_position.y - _text_height / 2, (int)_text_center_position.x + _text_width / 2, (int)_text_center_position.x + _text_height / 2);

		return{ { "hero_edge", font_rect, 0xFFFFFFFF, "N/A" } };
	}
	D3DXVECTOR2 get_image_size(){ return{ _image_width, _image_height }; }
private:

	RECT _get_window_rect(){
		auto window_handle = ::FindWindowEx(0, 0, 0, "Dota 2");
		RECT window_rect;
		GetWindowRect(window_handle, &window_rect);
		return window_rect;
	}

	// Image
	const float _image_width_percent = 0.077f;
	const float _image_height_percent = 0.077f;
	float _image_width;
	float _image_height;
	float _image_padding_top;
	float _image_padding_left;
	D3DXVECTOR2 _image_position;

	// Arrow
	const  float _arrow_width_percent = 0.008f;
	const  float _arrow_height_percent = 0.008f;
	float _arrow_width;
	float _arrow_height;
	D3DXVECTOR2 _arrow_center_position;

	// Dock
	const  float _dock_width_percent = 0.095f;
	const  float _dock_height_percent = 0.8f;
	float _dock_width;
	float _dock_height;
	float _dock_padding_top;
	D3DXVECTOR2 _dock_position;

	// Text
	const int _text_width_percent = 2;
	const int _text_height_percent = 2;
	int _text_width;
	int _text_height;
	int _text_padding_top;
	D3DXVECTOR2 _text_center_position;
};


