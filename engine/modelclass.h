#pragma once
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

#include <fstream>
//using namespace std;

#include "textureclass.h"
#include "objreader.h"

class ModelClass
{
private:
//The VertexType has replaced the color component with a texture coordinate component. The texture coordinates are now replacing the green color that was used in the previous tutorial.

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

	//struct ModelType
	//{
	//	float x, y, z;
	//	float tu, tv;
	//	float nx, ny, nz;
	//};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D10Device*, WCHAR*, WCHAR*);
	void Shutdown();
	void Render(ID3D10Device*);

	int GetIndexCount();
//The ModelClass now has a GetTexture function so it can pass its own texture resource to shaders that will draw this model.

	ID3D10ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D10Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D10Device*);
//ModelClass also now has both a private LoadTexture and ReleaseTexture for loading and releasing the texture that will be used to render this model.

	bool LoadTexture(ID3D10Device*, WCHAR*);
	void ReleaseTexture();
	bool LoadModel(char*);
	void ReleaseModel();

	

private:
	Vertices* m_model;
	ID3D10Buffer *m_vertexBuffer, *m_indexBuffer;
	ObjReader * read;
	int m_vertexCount, m_indexCount;
//The m_Texture variable is used for loading, releasing, and accessing the texture resource for this model.

	TextureClass* m_Texture;
};

#endif
