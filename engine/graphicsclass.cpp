#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_LightShader = 0;
	m_Light = 0;
	m_MouseDebugInfo = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}

GraphicsClass::~GraphicsClass()
{
	delete m_D3D;
	delete m_Camera;
	delete m_Model;
	m_LightShader = 0;
	m_Light = 0;
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
	D3DXMATRIX baseViewMatrix;
		
	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		throw exception();
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);
	
	// Create the model object.
	m_Model = new ModelClass;
	if(!m_Model)
	{
		return false;
	}

	m_ModelList = 0;
	m_Frustum = 0;
	
	//result = m_Model->Initialize(m_D3D->GetDevice(), "../Engine/data/cube.txt", L"../Engine/data/seafloor.dds");
	result = m_Model->Initialize(m_D3D->GetDevice(), "../Engine/data/sphere.txt", L"../Engine/data/seafloor.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		throw exception();
		return false;
	}

	// Create the light shader object.
	m_LightShader = new LightShaderClass;
	if(!m_LightShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}
	// The new light object is created here.

	// Create the light object.
	m_Light = new LightClass;
	if(!m_Light)
	{
		return false;
	}
	// The color of the light is set to purple and the light direction is set to point down the positive Z axis.

	// Initialize the light object.
	m_Light->SetAmbientColor(0.12f, 0.12f, 0.12f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 1.0f, 0.0f);

	//m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularPower(32.0f);

	// Create the text object.
	m_MouseDebugInfo = new TextClass;
	if(!m_MouseDebugInfo)
	{
		return false;
	}

	// Initialize the text object.
	result = m_MouseDebugInfo->Initialize(m_D3D->GetDevice(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object(m_MouseDebugInfo).", L"Error", MB_OK);
		return false;
	}

	m_ProcesorInfo = new TextClass;
	if(!m_MouseDebugInfo)
	{
		return false;
	}

	// Initialize the text object.
	result = m_ProcesorInfo->Initialize(m_D3D->GetDevice(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object(m_ProcesorInfo).", L"Error", MB_OK);
		return false;
	}
	m_RenderInfo = new TextClass;
	if(!m_MouseDebugInfo)
	{
		return false;
	}

	// Initialize the text object.
	result = m_RenderInfo->Initialize(m_D3D->GetDevice(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object(m_RenderInfo).", L"Error", MB_OK);
		return false;
	}

	// Create the model list object.
	m_ModelList = new ModelListClass;
	if(!m_ModelList)
	{
		return false;
	}

	// Initialize the model list object.
	result = m_ModelList->Initialize(25);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model list object.", L"Error", MB_OK);
		return false;
	}

	// Create the frustum object.
	m_Frustum = new FrustumClass;
	if(!m_Frustum)
	{
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	// Release the frustum object.
	if(m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = 0;
	}

	// Release the model list object.
	if(m_ModelList)
	{
		m_ModelList->Shutdown();
		delete m_ModelList;
		m_ModelList = 0;
	}

	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	if(m_MouseDebugInfo)
	{
		m_MouseDebugInfo->Shutdown();
		delete m_MouseDebugInfo;
		m_MouseDebugInfo = 0;
	}

	// Release the light object.
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the light shader object.
	if(m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// Release the model object.
	if(m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	return;
}

bool GraphicsClass::Frame(int mouseX, int mouseY)
{
	bool result;

	result = m_MouseDebugInfo->SetMousePosition(mouseX, mouseY);
	if(!result)
	{
		return false;
	}

	// Set the position of the camera.
	
	return true;
}

bool GraphicsClass::ShowDebugInfo(int mouseX,int mouseY,int fps,int cpuUsageInPerc, float time)
{
	bool result = true;

	result = m_MouseDebugInfo->SetMousePosition(mouseX, mouseY);
	if(!result)
	{
		return false;
	}
	result = m_RenderInfo->SetFps(fps);
	if(!result)
	{
		return false;
	}

	result = m_RenderInfo->SetTime(fps);
	if(!result)
	{
		return false;
	}

	// Set the cpu usage.
	result = m_ProcesorInfo->SetCpu(cpuUsageInPerc);
	if(!result)
	{
		return false;
	}


	return result;
}

void GraphicsClass::RotateCamera(float x, float y, float z)
{
	// Set the rotation of the camera.
	//x aukstis, y plotis, z gylis;
	m_Camera->SetRotation(x, y, z);
}

void GraphicsClass::switchMode(bool isFullScreen)
{
	m_D3D->switchmodes(isFullScreen);
}

bool GraphicsClass::Render()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	int modelCount, renderCount, index;
	float positionX, positionY, positionZ, radius;
	bool result = true, renderModels;
	D3DXVECTOR4 color;

	// Set the initial position of the camera.
	
	m_D3D->TurnZBufferOn();
	//render 3d
	m_Camera->Render();
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	
	

	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);
	modelCount = m_ModelList->GetModelCount();

	renderCount = 0;

	for(int index = 0; index< modelCount; index++)
	{
		m_ModelList->GetData(index, positionX, positionY, positionZ, color);
		radius = 1.0f;

		renderModels = m_Frustum->CheckSphere(positionX, positionY, positionZ, radius);

		// If it can be seen then render it, if not skip this model and check the next sphere.
		if(renderModels)
		{
			// Move the model to the location it should be rendered at.
			D3DXMatrixTranslation(&worldMatrix, positionX, positionY, positionZ); 

			// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
			m_Model->Render(m_D3D->GetDevice());

			// Render the model using the light shader.
			m_LightShader->Render(m_D3D->GetDevice(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
					      m_Model->GetTexture(), color, m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), 
			      m_Light->GetSpecularColor(), m_Light->GetSpecularPower());

			// Reset to the original world matrix.
			m_D3D->GetWorldMatrix(worldMatrix);

			// Since this model was rendered then increase the count for this frame.
			renderCount++;
		}
	}

	//rednder 2d
	m_D3D->TurnZBufferOff();

	result = m_ProcesorInfo->SetRenderCount(renderCount);
	if(!result)
	{
		return false;
	}

	m_MouseDebugInfo->Render(m_D3D->GetDevice(), worldMatrix, orthoMatrix);
	m_ProcesorInfo->Render(m_D3D->GetDevice(), worldMatrix, orthoMatrix);
	m_RenderInfo->Render(m_D3D->GetDevice(), worldMatrix, orthoMatrix);
	m_D3D->TurnZBufferOn();

	m_D3D->EndScene();
	return true;
}