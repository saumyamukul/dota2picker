#pragma once

#include <vector>
#include "asset_definitions.h"
#include "input_handler.h"

class Resources{
public:
	Resources(){

	}

	std::vector<TextureInfo> get_hud_texture_info(){
		std::vector<TextureInfo> texture_info = {
			{ "dock_frame", D3DXVECTOR2(dock_width, dock_height) },
			{ "left_arrow", D3DXVECTOR2(arrow_width, arrow_height) },
			{ "right_arrow", D3DXVECTOR2(arrow_width, arrow_height) },
			{ "default_image", D3DXVECTOR2(image_width, image_height) },
		};
		return texture_info;
	}

	std::vector<SpriteInfo> get_sprite_info(){
		auto window_rect = _get_window_rect();
		auto window_width = window_rect.right - window_rect.left;
		auto window_height = window_rect.bottom - window_rect.top;

		auto dock_padding_top = (window_height - dock_height) / 2.0f;
		auto image_padding_left = (dock_width - image_width) / 2.0f;
		auto image_padding_top = (dock_height - image_height) / 2.0f;
		auto arrows_padding_top = 20.0f;

		D3DXVECTOR2 dock_position = D3DXVECTOR2(window_width - dock_width, dock_padding_top);
		D3DXVECTOR2 image_position = D3DXVECTOR2(dock_position.x + image_padding_left, dock_position.y + image_padding_top);
		D3DXVECTOR2 arrow_center_position = D3DXVECTOR2(window_width - dock_width / 2.0f, image_position.y + image_height / 2.0f - arrow_height / 2.0f);

		auto arrow_offsets_from_center = image_width / 2.0f + 10.0f;

		D3DXVECTOR2 left_arrow_pos = arrow_center_position + D3DXVECTOR2(-arrow_offsets_from_center - arrow_width / 2.0f, 0.0f);
		D3DXVECTOR2 right_arrow_pos = arrow_center_position + D3DXVECTOR2(arrow_offsets_from_center - arrow_width / 2.0f, 0.0f);

		std::vector<SpriteInfo> sprite_info = {
			{ "dock_frame", "dock_frame", dock_position, D3DXVECTOR2(dock_width, dock_height) },
			{ "hero_image", "default_image", image_position, D3DXVECTOR2(image_width, image_height) },
			{ "left_arrow", "left_arrow", left_arrow_pos, D3DXVECTOR2(arrow_width, arrow_height) },
			{ "right_arrow", "right_arrow", right_arrow_pos, D3DXVECTOR2(arrow_width, arrow_height) },
		};
		return sprite_info;
	}

	std::vector<TextAsset> get_text_assets(){

		auto window_rect = _get_window_rect();
		auto window_width = window_rect.right - window_rect.left;
		auto window_height = window_rect.bottom - window_rect.top;

		auto dock_padding_top = (window_height - dock_height) / 2.0f;
		auto image_padding_left = (dock_width - image_width) / 2.0f;
		auto image_padding_top = (dock_height - image_height) / 2.0f;
		auto arrows_padding_top = 30.0f;

		D3DXVECTOR3 dock_position = D3DXVECTOR3(window_width - dock_width, dock_padding_top, 0);
		D3DXVECTOR3 image_position = D3DXVECTOR3(dock_position.x + image_padding_left, dock_position.y + image_padding_top, 0);
		D3DXVECTOR3 text_center_position = D3DXVECTOR3(window_width - dock_width / 2.0f, image_position.y + image_height + arrows_padding_top, 0);
		RECT font_rect;
		SetRect(&font_rect, text_center_position.x - 20.0f, text_center_position.y - 20.0f, text_center_position.x + 20.0f, text_center_position.x + 20.0f);

		return{ { "hero_edge", font_rect, 0xFFFFFFFF, "N/A" } };
	}
	D3DXVECTOR2 get_image_size(){ return{ image_width, image_height }; }
private:

	RECT _get_window_rect(){
		auto window_handle = ::FindWindowEx(0, 0, 0, "Dota 2");
		RECT window_rect;
		GetWindowRect(window_handle, &window_rect);
		return window_rect;
	}

	// Image
	const float image_width = 150.0f;
	const  float image_height = 84.0f;
	const float arrows_padding_top = 20.0f;

	// Arrow
	const  float arrow_width = 16.0f;
	const  float arrow_height = 16.0f;

	// Dock
	const  float dock_width = 200.0f;
	const  float dock_height = 1024.0f;
};


