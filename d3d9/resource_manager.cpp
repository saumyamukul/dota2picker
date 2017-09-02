#include "resource_manager.h"

#include "heroes.h"
#include <D3dx9core.h>
#include <vector>
#include "address_helpers.h"
#include "input_handler.h"
#include <string>
#include <iomanip> // setprecision
#include <iostream>
#include <sstream>
#include <assert.h>
#include "resources.h"

int current_hero_id = 0;
int vec_index = 0;

void ResourceManager::initialize(IDirect3DDevice9** device){
	_device = device;
	char result[MAX_PATH];
	_asset_path = std::string(result, GetModuleFileName(NULL, result, MAX_PATH));
	while (_asset_path.back() != '\\'){
		_asset_path.pop_back();
	}
	_asset_path += "Assets/"; 
	_create_textures();
	_create_sprites();
	_create_font();
	_create_text_assets();
}

void ResourceManager::_create_textures(){
	_create_hero_textures();
	_create_hud_textures();
}

void ResourceManager::_create_hero_textures(){
	for (auto hero : get_heroes()){
		std::string path = _asset_path + "Heroes/" + hero.second + ".png";
		LPDIRECT3DTEXTURE9 hero_texture = _create_texture(path, _resources.get_image_size().x, _resources.get_image_size().y);
		if (!hero_texture) continue;
		auto hero_name = hero.second;
		_textures.insert({ hero_name, hero_texture });
	}
}

void ResourceManager::_create_hud_textures(){
	auto hud_assets = _resources.get_hud_texture_info();
	for (auto hud_asset : hud_assets){
		std::string path = _asset_path + "HUD/" + hud_asset.name + ".png";
		LPDIRECT3DTEXTURE9 hud_texture = _create_texture(path, hud_asset.size.x, hud_asset.size.y);
		_textures.insert({ hud_asset.name, hud_texture });
	}

}

LPDIRECT3DTEXTURE9 ResourceManager::_create_texture(const std::string& path, float width, float height){
	LPDIRECT3DTEXTURE9 tex = nullptr;
	if (!SUCCEEDED(D3DXCreateTextureFromFileEx(*_device, path.c_str(), width, height, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &tex))){
		if (_textures.count("error")){
			return _textures["error"];
		}
	}
	return tex;
}



void ResourceManager::_create_sprites(){
	auto sprite_info = _resources.get_sprite_info();

	for (auto info : sprite_info){
		SpriteAsset asset;
		asset.sprite = _create_sprite();
		if (!asset.sprite) continue;
		asset.info = info;
		if (_textures.count(info.texture_name)){
			asset.texture = _textures[info.texture_name];
		}
		_sprites.insert({ info.sprite_name, asset });
	}
}

LPD3DXSPRITE ResourceManager::_create_sprite(){
	LPD3DXSPRITE sprite = nullptr;
	if (!SUCCEEDED(D3DXCreateSprite(*_device, &sprite))){
		assert(true);
	}
	return sprite;
}

void ResourceManager::_create_font(){
	HRESULT result = D3DXCreateFont(*_device, 22, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &_font);
}

void ResourceManager::_create_text_assets(){
	auto text_assets = _resources.get_text_assets();
	for (auto asset : text_assets){
		_text_assets.insert({ asset.name, asset });
	}
}