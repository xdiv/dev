#pragma once
#ifndef _MODEL3D_H_
#define _MODEL3D_H_

// INCLUDES //
#include <d3d10.h>
#include <d3dx10math.h>
#include "Base3DModel.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: Model3d
////////////////////////////////////////////////////////////////////////////////
class Model3d : public Base3DModel
{
private:

	//Here is the definition of our vertex type that will be used with the vertex buffer in this ModelClass. Also take note that this typedef must match the layout in the ColorShaderClass that will be looked at later in the tutorial.

public:
	Model3d();
	Model3d(const Model3d&);
	~Model3d();
//The functions here handle initializing and shutdown of the model's vertex and index buffers. The Render function puts the model geometry on the video card to prepare it for drawing by the color shader.

	bool Initialize(ID3D10Device*);
	void Shutdown();
	void Render(ID3D10Device*);
	void CreateTriangle();
	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D10Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D10Device*);
	
//The private variables in the ModelClass are the vertex and index buffer as well as two integers to keep track of the size of each buffer. Note that all DirectX 10 buffers generally use the generic ID3D10Buffer type and are more clearly identified by a buffer description when they are first created.

private:
	ID3D10Buffer *m_vertexBuffer, *m_indexBuffer;
};

#endif

