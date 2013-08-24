#include "Base3DModel.h"

Base3DModel::Base3DModel()
{
	vertices = 0;
	indices = 0;
	vertexCount, indexCount = 0;
	m_Texture = 0;
}

Base3DModel::~Base3DModel()
{
	delete[] vertices;
	delete[] indices;
	vertices = 0;
	indices = 0;
	//texturu netrinam, jos pasirupina savim paèios
	m_Texture = 0;
}
//Base3DModel::Base3DModel(const Base3DModel & base3DModel)
//{
//}

Base3DModel::Base3DModel(VertexType * vertices, unsigned long * indices, int m_vertexCount, int m_indexCount)
{
	vertices = 0;
	indices = 0;
	this->indexCount = m_indexCount;
	this->vertexCount = m_vertexCount;
	this->vertices = vertices;
	this->indices = indices;
}

void Base3DModel::GetVertex(VertexType & vertices, unsigned long & indices, int & m_vertexCount, int & m_indexCount)
{
	vertices = *this->vertices;
	indices = *this->indices;
	m_vertexCount = this->vertexCount;
	m_indexCount = this->indexCount;
}

void Base3DModel::SetTexture(TextureClass *  textures)
{
	m_Texture = textures;
}

ID3D10ShaderResourceView* Base3DModel::GetTexture()
{
	return m_Texture->GetTexture();
}



