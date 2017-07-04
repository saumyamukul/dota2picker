#pragma once

#include <d3d9.h>
#include <map>
#include <D3dx9core.h>
#include <vector>
#include <functional>

struct Vec2{
	int x;
	int y;
};

struct TextureAsset{
	std::string name;
	Vec2 size;
};

struct SpriteAsset{
	std::string name;
	D3DXVECTOR3 position;
	LPD3DXSPRITE sprite;
	LPDIRECT3DTEXTURE9 texture;
	Vec2 size;
	std::function<void()> callback;
};

class ResourceManager{
public:
	static bool load_textures(IDirect3DDevice9** device);
	static bool create_sprites(IDirect3DDevice9** device);
	static std::vector<SpriteAsset>* get_sprites();
	static std::map<std::string, LPDIRECT3DTEXTURE9> get_textures(){ return _textures; }
private:
	static std::map<std::string, LPDIRECT3DTEXTURE9> _textures;
	static std::vector<SpriteAsset> _sprites;
};