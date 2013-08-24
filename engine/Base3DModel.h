#pragma once
#ifndef _BASE3DMODEL_H_
#define _BASE3DMODEL_H_

// INCLUDES //
#include <d3d10.h>
#include <vector>
#include <d3dx10math.h>
#include "textureclass.h"

using namespace std;

struct VertexType
{
	D3DXVECTOR3 position;
	D3DXVECTOR4 color;
	D3DXVECTOR2 texture;
};

class Base3DModel
{
public:
	Base3DModel();
	Base3DModel(VertexType * vertices, unsigned long * indices, int m_vertexCount, int m_indexCount);
	~Base3DModel();

	//The ModelClass now has a GetTexture function so it can pass its own texture resource to shaders that will draw this model.
	ID3D10ShaderResourceView* GetTexture();

public:
	void GetVertex(VertexType & vertices, unsigned long & indices, int & m_vertexCount, int & m_indexCount);
	void SetTexture(TextureClass *  textures);

protected:
	//Virðûnës
	VertexType * vertices;
	//Briaunos
	unsigned long * indices;
	int vertexCount, indexCount;
	//The m_Texture variable is used for loading, releasing, and accessing the texture resource for this model.
	TextureClass* m_Texture;

};

#endif

