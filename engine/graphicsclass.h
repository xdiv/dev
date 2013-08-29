#pragma once

#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

#include <windows.h>

#include "d3dclass.h"
#include "cameraclass.h"
#include "textclass.h"
#include "modelclass.h"
#include "lightshaderclass.h"
#include "lightclass.h"
#include "modellistclass.h"
#include "frustumclass.h"

// GLOBALS //
const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 10000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(int , int);
	void switchMode(bool isFullScreen);

	bool ShowDebugInfo(int ,int ,int ,int , float);
	bool Render();
	void RotateCamera(float, float ,float); 

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	LightShaderClass* m_LightShader;
	LightClass* m_Light;

	TextClass* m_MouseDebugInfo;
	TextClass* m_ProcesorInfo;
	TextClass* m_RenderInfo;

	ModelListClass* m_ModelList;
	FrustumClass* m_Frustum;
};

#endif

