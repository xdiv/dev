////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	//m_ColorShader = 0;
	m_TextureShader = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
	delete m_D3D;
	delete m_Camera;
	delete m_Model;
	//delete m_ColorShader;
	delete m_TextureShader;
	m_TextureShader = 0;

	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

		
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
	
	// Create the model object.
	m_Model = new ModelClass;
	if(!m_Model)
	{
		return false;
	}
	
	//m_Model->CreateTriangle();
	//TextureClass * textures = new TextureClass();
	////textures->Initialize(m_D3D->GetDevice(), L"../engine/data/black.dds");
	//textures->Initialize(m_D3D->GetDevice(), L"C:/git/engine/engine/seafloor.dds");

	//m_Model->SetTexture(textures);
	//// Initialize the model object.
	result = m_Model->Initialize(m_D3D->GetDevice(), L"../Engine/data/back.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		throw exception();
		return false;
	}

	// Create the color shader object.
	/*m_ColorShader = new ColorShaderClass;
	if(!m_ColorShader)
	{
		return false;
	}*/
	m_TextureShader = new TextureShaderClass;
	if(!m_TextureShader)
	{
		return false;
	}

	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!m_TextureShader)
	{
		MessageBox(hwnd, L"Could not initialize the TextureShaderClass object.", L"Error", MB_OK);
		throw exception();
		return false;
	}

	// Initialize the color shader object.
	/*result = m_ColorShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		throw exception();
		return false;
	}*/
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

	// Release the color shader object.
	/*if(m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}*/
	// Release the texture shader object.
	if(m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
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


bool GraphicsClass::Frame()
{
	bool result;


	// Render the graphics scene.
	result = Render();
	if(!result)
	{
		return false;
	}
	return true;
}

void GraphicsClass::switchMode(bool isFullScreen)
{
	m_D3D->switchmodes(isFullScreen);
}


bool GraphicsClass::Render()
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix;

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_D3D->GetDevice());

	//// Render the model using the color shader.
	//m_ColorShader->Render(m_D3D->GetDevice(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);

	// The texture shader is called now instead of the color shader to render the model. Notice it also takes the texture resource pointer 
	// from the model so the texture shader has access to the texture from the model object.
	// Render the model using the texture shader.
	m_TextureShader->Render(m_D3D->GetDevice(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());


	// Present the rendered scene to the screen.
	m_D3D->EndScene();
	return true;
}