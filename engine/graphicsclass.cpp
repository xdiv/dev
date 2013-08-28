#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_LightShader = 0;
	m_Light = 0;
	m_Bitmap = 0;
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
	delete m_Bitmap ;
	m_LightShader = 0;
	m_Light = 0;
	m_Bitmap = 0;
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

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();
	
	// Create the model object.
	m_Model = new ModelClass;
	if(!m_Model)
	{
		return false;
	}

	m_Camera->GetViewMatrix(baseViewMatrix);
	
	result = m_Model->Initialize(m_D3D->GetDevice(), "../Engine/data/cube.txt", L"../Engine/data/seafloor.dds");
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
	m_Light->SetDirection(1.0f, 0.0f, 0.0f);

	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularPower(32.0f);

	// Create the bitmap object.
	m_Bitmap = new BitmapClass;
	if(!m_Bitmap)
	{
		return false;
	}

	// Initialize the bitmap object.
	result = m_Bitmap->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"../Engine/data/seafloor.dds", 256, 256);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}

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

	return true;
}

void GraphicsClass::Shutdown()
{
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	// Release the bitmap object.
	if(m_Bitmap)
	{
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
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

void GraphicsClass::switchMode(bool isFullScreen)
{
	m_D3D->switchmodes(isFullScreen);
}

bool GraphicsClass::Render()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	D3DXMATRIX camerViewMatrix;
	bool result = true;

	//render 3d
	m_D3D->TurnZBufferOn();

	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	
	m_Camera->Render();

	m_Camera->GetViewMatrix(viewMatrix);
	
	m_D3D->GetWorldMatrix(worldMatrix);

	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	//rednder 2d
	m_D3D->TurnZBufferOff();

	m_MouseDebugInfo->Render(m_D3D->GetDevice(), worldMatrix, orthoMatrix);
	m_ProcesorInfo->Render(m_D3D->GetDevice(), worldMatrix, orthoMatrix);
	m_RenderInfo->Render(m_D3D->GetDevice(), worldMatrix, orthoMatrix);

	m_D3D->EndScene();
	return true;
}