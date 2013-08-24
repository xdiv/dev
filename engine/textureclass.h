#pragma once

// The TextureClass encapsulates the loading, unloading, and accessing of a single texture resource. 
// For each texture needed an object of this class must be instantiated.
#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_

#include <d3d10.h>
#include <d3dx10.h>
#include "global.h"


class TextureClass
{
public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();
	
	//The first two functions will load a texture from a given file name and unload that texture when it is no longer needed.
	bool Initialize(ID3D10Device*, WCHAR*);
	void Shutdown();
	
	//The GetTexture function returns a pointer to the texture resource so that it can be used for rendering by shaders.
	ID3D10ShaderResourceView* GetTexture();

private:
	// This is the private texture resource.
	ID3D10ShaderResourceView* m_texture;
};

#endif
