#pragma once
#ifndef _TEXTCLASS_H_
#define _TEXTCLASS_H_

#include "fontclass.h"
#include "fontshaderclass.h"

class TextClass
{
private:

	//SentenceType is the structure that holds the rendering information for each text sentence.
	struct SentenceType
	{
		ID3D10Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	//The VertexType must match the one in the FontClass.
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool Initialize(ID3D10Device*, HWND, int, int, D3DXMATRIX);
	void Shutdown();
	void Render(ID3D10Device*, D3DXMATRIX, D3DXMATRIX);
	bool AddWord(char * text, int x, int y, float r, float g, float b );
	bool SetMousePosition(int, int);

private:
	bool InitializeSentence(SentenceType**, int, ID3D10Device*);
	bool UpdateSentence(SentenceType*, char*, int, int, float, float, float);
	void ReleaseSentence(SentenceType**);
	void RenderSentence(ID3D10Device*, SentenceType*, D3DXMATRIX, D3DXMATRIX);

private:
	FontClass* m_Font;
	FontShaderClass* m_FontShader;
	int m_screenWidth, m_screenHeight;
	D3DXMATRIX m_baseViewMatrix;
	ID3D10Device * m_d3d;

	// We will use two sentences in this tutorial.

	SentenceType* m_sentence1;
	SentenceType* m_sentence2;
};

#endif

