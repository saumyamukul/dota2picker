#pragma once

#include <d3d9.h>
#include <map>
#include <D3dx9core.h>
#include <vector>

struct TextureAsset{
	std::string name;
	int width;
	int height;
};

struct SpriteAsset{
	std::string name;
	D3DXVECTOR3 position;
	LPD3DXSPRITE sprite;
	LPDIRECT3DTEXTURE9 texture;
};

class TextureLoader{
public:
	static bool load_textures(IDirect3DDevice9** device);
	static bool create_sprites(IDirect3DDevice9** device);
	static std::vector<SpriteAsset> get_sprites();
private:
	static std::map<std::string, LPDIRECT3DTEXTURE9> _textures;
	static std::vector<SpriteAsset> _sprites;
};