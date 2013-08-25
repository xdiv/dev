#pragma once
#ifndef _LIGHTSHADERCLASS_H_
#define _LIGHTSHADERCLASS_H_

// INCLUDES //
#include <d3d10.h>
#include <d3dx10math.h>
#include <fstream>
using namespace std;

// Class name: LightShaderClass
class LightShaderClass
{
public:
	LightShaderClass();
	LightShaderClass(const LightShaderClass&);
	~LightShaderClass();

	bool Initialize(ID3D10Device*, HWND);
	void Shutdown();
	void Render(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
			      ID3D10ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor);

private:
	bool InitializeShader(ID3D10Device*, HWND, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	void SetShaderParameters(D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4 );
	void RenderShader(ID3D10Device*, int);

private:
	ID3D10Effect* m_effect;
	ID3D10EffectTechnique* m_technique;
	ID3D10InputLayout* m_layout;

	ID3D10EffectMatrixVariable* m_worldMatrixPtr;
	ID3D10EffectMatrixVariable* m_viewMatrixPtr;
	ID3D10EffectMatrixVariable* m_projectionMatrixPtr;
	ID3D10EffectShaderResourceVariable* m_texturePtr;
	// There are two new private pointers for the direction of the light and the diffuse color of the light. These will be used by this
	// class to set the two new global light variables inside the HLSL shader.

	ID3D10EffectVectorVariable* m_lightDirectionPtr;
	ID3D10EffectVectorVariable* m_diffuseColorPtr;
	ID3D10EffectVectorVariable* m_ambientColorPtr;
};

#endif
