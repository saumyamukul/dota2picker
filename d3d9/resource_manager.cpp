#include "resource_manager.h"

#include "heroes.h"
#include "error_handler.h"
#include <D3dx9core.h>
#include <vector>
#include "address_helpers.h"
#include "utils.h"

float image_width = 150.0f;
float image_height = 84.0f;
int arrow_width = 16;
int arrow_height = 16;
int dock_width = 200;
int dock_height = 1024;

std::map<std::string, LPDIRECT3DTEXTURE9> ResourceManager::_textures;
std::vector<SpriteAsset> ResourceManager::_sprites;
std::map<std::string, LPDIRECT3DTEXTURE9>::iterator image_iter;
bool ResourceManager::load_textures(IDirect3DDevice9** device){
	std::string error_message;

	{
		// Load hero textures
		for (auto hero : heroes){
			std::string file_path = "C:/Users/saumyamukul/Documents/Visual Studio 2013/Projects/DotaDLL/d3d9/Assets/Heroes/" + hero.second + ".png";
			LPDIRECT3DTEXTURE9 tex;
			if (!SUCCEEDED(D3DXCreateTextureFromFileEx(*device, file_path.c_str(), image_width, image_height, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &tex))){
				error_message = "Texture loading failed.\n";
				goto on_error;
			}
			_textures.insert({ hero.second, tex });
		}
	}

	{
		// Load HUD textures
		std::vector<TextureAsset> hud_assets = {
			{ "DockFrame", dock_width, dock_height },
			{ "LeftArrow", arrow_width, arrow_height },
			{ "RightArrow", arrow_width, arrow_height }
		};

		for (auto asset : hud_assets){
			LPDIRECT3DTEXTURE9 tex;
			auto file_path = "C:/Users/saumyamukul/Documents/Visual Studio 2013/Projects/DotaDLL/d3d9/Assets/" + asset.name + ".png";
			if (!SUCCEEDED(D3DXCreateTextureFromFileEx(*device, file_path.c_str(), asset.size.x, asset.size.y, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &tex))){
				error_message = "Texture loading failed.\n";
				goto on_error;
			}
			_textures.insert({ asset.name, tex });
		}
	}
	create_sprites(device);
	return true;


on_error:
	ErrorHandler::log_error(error_message);
	return false;
}

bool ResourceManager::create_sprites(IDirect3DDevice9** device){
	std::string error_message;

	// Calculate HUD sprite dimensions
	// Could be cleaned up by coming up with an alignment system where you have a parent/child hierarchy
	// and each child can be assigned an orientation (LEFT, RIGHT, TOP, BOTTOM, CUSTOM) w.r.t its parent
	auto window_rect = Utils::get_window_rect();
	auto window_width = window_rect.right - window_rect.left;
	auto window_height = window_rect.bottom - window_rect.top;

	auto dock_padding_top = (window_height - dock_height) / 2.0f;
	auto image_padding_left = (dock_width - image_width) / 2.0f;
	auto image_padding_top = (dock_height - image_height) / 2.0f;
	auto arrows_padding_top = 20.0f;

	D3DXVECTOR3 dock_position = D3DXVECTOR3(window_width - dock_width, dock_padding_top, 0);
	D3DXVECTOR3 image_position = D3DXVECTOR3(dock_position.x + image_padding_left, dock_position.y + image_padding_top, 0);
	D3DXVECTOR3 arrow_center_position = D3DXVECTOR3(window_width - dock_width / 2.0f, image_position.y + image_height + arrows_padding_top, 0);

	auto arrow_offsets_from_center = 20.0f;

	D3DXVECTOR3 left_arrow_pos = image_position + D3DXVECTOR3(-arrow_offsets_from_center - arrow_width / 2.0f, 0.0f, 0.0f);
	D3DXVECTOR3 right_arrow_pos = arrow_center_position + D3DXVECTOR3(arrow_offsets_from_center - arrow_width / 2.0f, 0.0f, 0.0f);

	image_iter = _textures.begin();
	auto image_tex = (image_iter != _textures.end()) ? image_iter->second : nullptr;

	auto left_arrow_func = []{
		auto textures = ResourceManager::get_textures();
		if (image_iter != textures.begin()){
			image_iter--;
			auto sprites = ResourceManager::get_sprites();
			(*sprites)[1].texture = image_iter->second;
		}
	};
	auto right_arrow_func = []{
		auto textures = ResourceManager::get_textures();
		if (std::next(image_iter) != textures.end()){
			image_iter++;
			auto sprites = ResourceManager::get_sprites();
			(*sprites)[1].texture = image_iter->second;
		}
	};

	std::vector<SpriteAsset> assets = {
		{ "DockFrame", dock_position, nullptr, nullptr, { dock_width, dock_height }, nullptr },
		{ "Image", image_position, nullptr, _textures["abaddon"], { image_width, image_height }, nullptr },
		{ "LeftArrow", left_arrow_pos, nullptr, nullptr, { arrow_width, arrow_height }, left_arrow_func },
		{ "RightArrow", right_arrow_pos, nullptr, nullptr, { arrow_width, arrow_height }, right_arrow_func }
	};
	for (auto asset : assets){
		LPD3DXSPRITE sprite;
		if (!SUCCEEDED(D3DXCreateSprite(*device, &sprite))){
			std::string error_message;
			goto on_sprite_error;
		}
		if (!asset.texture){
			auto tex = _textures.find(asset.name);
			if (tex != _textures.end()){
				asset.texture = tex->second;
			}
		}
		asset.sprite = sprite;
		_sprites.push_back(asset);
	}
	return true;

on_sprite_error:
	ErrorHandler::log_error(error_message);
	return false;
}

std::vector<SpriteAsset>* ResourceManager::get_sprites(){
	return &_sprites;
}
//
////A pre-formatted string showing the current frames per second
//HRESULT result = D3DXCreateFont(*ppReturnedDeviceInterface,     //D3D Device
//
//	22,               //Font height
//
//	0,                //Font width
//
//	FW_NORMAL,        //Font Weight
//
//	1,                //MipLevels
//
//	false,            //Italic
//
//	DEFAULT_CHARSET,  //CharSet
//
//	OUT_DEFAULT_PRECIS, //OutputPrecision
//
//	ANTIALIASED_QUALITY, //Quality
//
//	DEFAULT_PITCH | FF_DONTCARE,//PitchAndFamily
//
//	"Arial",          //pFacename,
//
//	&m_font);         //ppFont
//return hr;