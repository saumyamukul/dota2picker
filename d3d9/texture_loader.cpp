#include "texture_loader.h"

#include "heroes.h"
#include "error_handler.h"
#include <D3dx9core.h>
#include <vector>
#include "address_helpers.h"

float image_width = 150.0f;
float image_height = 84.0f;
int arrow_width = 16;
int arrow_height = 16;
int dock_width = 200;
int dock_height = 1024;

std::map<std::string, LPDIRECT3DTEXTURE9> TextureLoader::_textures;
std::vector<SpriteAsset> TextureLoader::_sprites;
std::map<std::string, LPDIRECT3DTEXTURE9>::iterator image_iter;
bool TextureLoader::load_textures(IDirect3DDevice9** device){
	std::vector<TextureAsset> assets = {
		{ "DockFrame", dock_width, dock_height },
		{ "LeftArrow", arrow_width, arrow_height },
		{ "RightArrow", arrow_width, arrow_height }
	};

	std::string error_message;
	for (auto hero : heroes){
		std::string file_path = "C:/Users/saumyamukul/Documents/Visual Studio 2013/Projects/DotaTrueStrike/Images/Full-Images/" + hero.second + ".png";
		LPDIRECT3DTEXTURE9 tex;
		if (!SUCCEEDED(D3DXCreateTextureFromFileEx(*device, file_path.c_str(), image_width, image_height, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &tex))){
			error_message = "Texture loading failed.\n";
			goto on_error;
		}
		_textures.insert({ hero.second, tex });
	}

	for (auto asset : assets){
		LPDIRECT3DTEXTURE9 tex;
		auto file_path = "C:/Users/saumyamukul/Documents/Visual Studio 2013/Projects/DotaDLL/d3d9/Assets/" + asset.name + ".png";
		if (!SUCCEEDED(D3DXCreateTextureFromFileEx(*device, file_path.c_str(), asset.size.x, asset.size.y, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &tex))){
			error_message = "Texture loading failed.\n";
			goto on_error;
		}
		_textures.insert({ asset.name, tex });
	}
	create_sprites(device);
	return true;


on_error:
	ErrorHandler::log_error(error_message);
	return false;
}

bool TextureLoader::create_sprites(IDirect3DDevice9** device){
	std::string error_message;
	auto window_handle = GetWindow();
	RECT window_rect;
	GetWindowRect(window_handle, &window_rect);
	auto window_width = window_rect.right - window_rect.left;
	auto window_height = window_rect.bottom - window_rect.top;

	D3DXVECTOR3 dockPosition = D3DXVECTOR3(window_width - dock_width, (window_height - dock_height) / 2.0f, 0);
	D3DXVECTOR3 image_position = D3DXVECTOR3(window_width - dock_width + (dock_width - image_width) / 2.0f, (window_height - dock_height) / 2.0f + (dock_height - image_height) / 2.0f, 0);
	D3DXVECTOR3 arrow_position = D3DXVECTOR3(window_width - dock_width / 2.0f, (window_height - dock_height) / 2.0f + (dock_height - image_height) / 2.0f + image_height + 20.0f, 0);

	D3DXVECTOR3 left_pos = arrow_position + D3DXVECTOR3(-20 - arrow_width / 2.0f, 0.0f, 0.0f);
	// Draw right arrow
	D3DXVECTOR3 right_pos = arrow_position + D3DXVECTOR3(20 - arrow_width / 2.0f, 0.0f, 0.0f);
	image_iter = _textures.begin();
	auto image_tex = (image_iter != _textures.end()) ? image_iter->second : nullptr;

	auto left_arrow_func = []{
		auto textures = TextureLoader::get_textures();
		if (image_iter != textures.begin()){
			image_iter--;
			auto sprites = TextureLoader::get_sprites();
			(*sprites)[1].texture = image_iter->second;
		}
	};
	auto right_arrow_func = []{
		auto textures = TextureLoader::get_textures();
		if (std::next(image_iter) != textures.end()){
			image_iter++;
			auto sprites = TextureLoader::get_sprites();
			(*sprites)[1].texture = image_iter->second;
		}
	};

	std::vector<SpriteAsset> assets = {
		{ "DockFrame", dockPosition, nullptr, nullptr, { dock_width, dock_height }, nullptr },
		{ "Image", image_position, nullptr, _textures["abaddon"], { image_width, image_height }, nullptr },
		{ "LeftArrow", left_pos, nullptr, nullptr, { arrow_width, arrow_height }, left_arrow_func },
		{ "RightArrow", right_pos, nullptr, nullptr, { arrow_width, arrow_height }, right_arrow_func }
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

std::vector<SpriteAsset>* TextureLoader::get_sprites(){
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