#pragma once

#include <string>
#include <d3d9.h>
#include <D3dx9core.h>
#include <functional>

struct TextureInfo{
	std::string name;
	D3DXVECTOR2 size;
};

struct SpriteInfo{
	std::string sprite_name;
	std::string texture_name;
	D3DXVECTOR2 position;
	D3DXVECTOR2 size;
};

struct SpriteAsset{
	SpriteInfo info;
	LPD3DXSPRITE sprite;
	LPDIRECT3DTEXTURE9 texture;
};

struct TextAsset{
	std::string name;
	RECT rect;
	D3DCOLOR color;
	std::string text;
};
