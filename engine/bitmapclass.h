#pragma once
#ifndef _BITMAPCLASS_H_
#define _BITMAPCLASS_H_

#include "textureclass.h"

class BitmapClass
{
private:
	//Each bitmap image is still a polygon object that gets rendered similar to 3D objects. For 2D images we just need a position vector 
	//and texture coordinates.
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	BitmapClass();
	BitmapClass(const BitmapClass&);
	~BitmapClass();

	bool Initialize(ID3D10Device*, int, int, WCHAR*, int, int);
	void Shutdown();
	bool Render(ID3D10Device*, int, int);

	int GetIndexCount();
	ID3D10ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D10Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(int, int);
	void RenderBuffers(ID3D10Device*);

	bool LoadTexture(ID3D10Device*, WCHAR*);
	void ReleaseTexture();

private:
	ID3D10Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	TextureClass* m_Texture;
	
	// The BitmapClass will need to maintain some extra information that a 3D model wouldn't such as the screen size, the bitmap size, and 
	// the last place it was rendered. We have added extra private variables here to track that extra information.
	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;
};

#endif
