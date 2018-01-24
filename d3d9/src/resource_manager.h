#pragma once

#include <d3d9.h>
#include <map>
#include <D3dx9core.h>
#include <vector>
#include "asset_definitions.h"
#include "resources.h"

class ResourceManager{
public:
	static ResourceManager* get_instance(){
		static ResourceManager instance;
		return &instance;
	}
	void initialize(IDirect3DDevice9** device);
	std::map<std::string, LPDIRECT3DTEXTURE9>* get_texture_map(){ return &_textures; }
	std::map<std::string, SpriteAsset>* get_sprite_asset_map(){ return &_sprites; }
	std::map<std::string, TextAsset>* get_text_asset_map(){ return &_text_assets; }
	LPD3DXFONT get_default_font(){ return _font; }
private:
	ResourceManager(){}

	void _create_font();
	// Texture creation
	void _create_textures();
	void _create_hero_textures();
	void _create_hud_textures();
	LPDIRECT3DTEXTURE9 _create_texture(const std::string& path, int width, int height);

	// Sprite creation
	void _create_sprites();
	LPD3DXSPRITE _create_sprite();

	// Text asset creation
	void _create_text_assets();

	Resources _resources;
	IDirect3DDevice9** _device;
	LPD3DXFONT _font;
	std::map<std::string, LPDIRECT3DTEXTURE9> _textures;
	std::map<std::string, SpriteAsset> _sprites;
	std::map<std::string, TextAsset> _text_assets;
	std::string _asset_path;
};