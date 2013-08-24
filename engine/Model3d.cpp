#include "Model3d.h"


Model3d::Model3d(void) : Base3DModel()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	vertexCount = 0;
	indexCount = 0;
}

Model3d::Model3d(const Model3d & Model3d) : Base3DModel()
{
}

Model3d::~Model3d(void)
{
}

void Model3d::CreateTriangle()
{
	vertexCount = 3;
	indexCount = 3;

	// Create the vertex array.
	vertices = new VertexType[vertexCount];
	// Create the index array.
	indices = new unsigned long[indexCount];

	// Now fill both the vertex and index array with the three points of the model as well as the index to each of the points. 
	// Please note that I create the points in the clockwise order of drawing them. If you do this counter clockwise it will think the 
	// model is facing the opposite direction and not draw it due to back face culling. Always remember that the order in which you send 
	// your vertices to the GPU is very important! The color is set here as well since it is part of the vertex description. I set the color to green.

	// Load the vertex array with data.
	vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
	vertices[0].texture = D3DXVECTOR2(0.0f, 1.0f);

	vertices[1].position = D3DXVECTOR3(0.0f, 1.0f, 0.0f);  // Top middle.
	vertices[1].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
	vertices[0].texture = D3DXVECTOR2(0.5f, 0.0f);

	vertices[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[2].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
	vertices[0].texture = D3DXVECTOR2(1.0f, 1.0f);

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.
}

//The Initialize function will call the initialization functions for the vertex and index buffers.
bool Model3d::Initialize(ID3D10Device* device)
{
	bool result;


	// Initialize the vertex and index buffer that hold the geometry for the model.
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}
	return true;
}

//The Shutdown function will call the shutdown functions for the vertex and index buffers.
void Model3d::Shutdown()
{
	// Release the vertex and index buffers.
	ShutdownBuffers();

	return;
}

//Render is called from the GraphicsClass::Render function. This function calls RenderBuffers to put the vertex and index buffers on the 
//graphics pipeline so the color shader will be able to render them.
void Model3d::Render(ID3D10Device* device)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(device);

	return;
}

//GetIndexCount returns the number of indexes in the model. The color shader will need this information to draw this model.
int Model3d::GetIndexCount()
{
	return indexCount;
}

//The InitializeBuffers function is where we handle creating the vertex and index buffers. Usually you would read in a model and create 
//the buffers from that data file. For this tutorial we will just set the three points in the vertex and index buffer manually since it is only a single model.
bool Model3d::InitializeBuffers(ID3D10Device* device)
{

	D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D10_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	//First create two temporary arrays to hold the vertex and index data that we will use later to populate the final buffers with.

	// With the vertex array and index array filled out we can now use those to create the vertex buffer and index buffer. Creating both 
	// buffers is done in the same fashion. First fill out a description of the buffer. In the description the ByteWidth 
	// (size of the buffer) and the BindFlags (type of buffer) are what you need to ensure are filled out correctly. After the description
	// is filled out you need to also fill out a subresource pointer which will point to either your vertex or index array you previously
	// created. With the description and subresource pointer you can call CreateBuffer using the D3D device and it will return a 
	// pointer to your new buffer.

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;

	// Now finally create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// After the vertex buffer and index buffer have been created you can delete the vertex and index arrays as they are no longer needed since the data was copied into the buffers.
	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}

//The ShutdownBuffers function just releases the vertex buffer and index buffer that were created in the InitializeBuffers function.
void Model3d::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

//RenderBuffers is called from the Render function. The purpose of this function is to set the vertex buffer and index buffer as active 
//on the input assembler in the GPU. Once the GPU has an active vertex buffer it can then use the HLSL shader we wrote to render that buffer. 
//This function also defines how those buffers should be drawn such as models, lines, fans, and so forth. In this tutorial we set the 
//vertex buffer and index buffer as active on the input assembler and tell the GPU that the buffers should be drawn as models using 
//the IASetPrimitiveTopology DirectX function.
void Model3d::RenderBuffers(ID3D10Device* device)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	device->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	device->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case models.
	device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

