#include "d3dclass.h"


D3DClass::D3DClass()
{
	m_device = 0;
	m_swapChain = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
	displayModeList = 0;
}


D3DClass::D3DClass(const D3DClass& other)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
}


D3DClass::~D3DClass()
{
	delete	m_device;
	delete	m_swapChain;
	delete	m_renderTargetView;
	delete	m_depthStencilBuffer;
	delete	m_depthStencilState;
	delete	m_depthStencilView;
	delete	m_rasterState;
	delete[] displayModeList;

	m_device = 0;
	m_swapChain = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
	displayModeList = 0;
}

void D3DClass::switchmodes(bool fullscreen)
{
	m_swapChain->SetFullscreenState(fullscreen, NULL);
}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator, stringLength;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ID3D10Texture2D* backBufferPtr;
	D3D10_TEXTURE2D_DESC depthBufferDesc;
	D3D10_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D10_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D10_VIEWPORT viewport;
	float fieldOfView, screenAspect;
	D3D10_RASTERIZER_DESC rasterDesc;


	// Store the vsync setting.
	m_vsync_enabled = vsync;
	// Before we can initialize Direct3D we have to get the refresh rate from the video card/monitor. Each computer may 
	// be slightly different so we will need to query for that information. We query for the numerator and denominator 
	// values and then pass them to DirectX during the setup and it will calculate the proper refresh rate.
	// If we don't do this and just set the refresh rate to a default value which may not exist on all computers then 
	// DirectX will respond by performing a blit instead of a buffer 
	// flip which will degrade performance and give us annoying errors in the debug output.

	// Create a DirectX graphics interface factory.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if(FAILED(result))
	{
		return false;
	}

	// Use the factory to create an adapter for the primary graphics interface (video card).
	result = factory->EnumAdapters(0, &adapter);
	if(FAILED(result))
	{
		return false;
	}

	// Enumerate the primary adapter output (monitor).
	result = adapter->EnumOutputs(0, &adapterOutput);
	if(FAILED(result))
	{
		return false;
	}

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if(!displayModeList)
	{
		return false;
	}

	// Now fill the display mode list structures.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if(FAILED(result))
	{
		return false;
	}

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for(i=0; i<numModes; i++)
	{
		if(displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if(displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}
	//We now have the numerator and denominator for the refresh rate. The last thing we will retrieve using the adapter 
	//is the name of the video card and the amount of memory on the video card.

	// Get the adapter (video card) description.
	result = adapter->GetDesc(&adapterDesc);
	if(FAILED(result))
	{
		return false;
	}

	// Store the dedicated video card memory in megabytes.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if(error != 0)
	{
		return false;
	}
	//Now that we have stored the numerator and denominator for the refresh rate and the video card information 
	//we can release the structures and interfaces used to get that information.

	// Release the display mode list.
	delete [] displayModeList;
	displayModeList = 0;

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = 0;

	// Release the adapter.
	adapter->Release();
	adapter = 0;

	// Release the factory.
	factory->Release();
	factory = 0;

	// Now that we have the refresh rate from the system we can start the DirectX initialization. 
	// The first thing we'll do is fill out the description of the swap chain. 
	// The swap chain is the front and back buffer to which the graphics will be drawn.
	// Generally you use a single back buffer, do all your drawing to it, and then swap it to the front buffer
	// which then displays on the user's screen. That is why it is called a swap chain.

	// Initialize the swap chain description.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set to a single back buffer.
	swapChainDesc.BufferCount = 1;

	// Set the width and height of the back buffer.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// Set regular 32-bit surface for the back buffer.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// The next part of the description of the swap chain is the refresh rate. The refresh rate is how many times a 
	// second it draws the back buffer to the front buffer. If vsync is set to true in our graphicsclass.h header 
	// then this will lock the refresh rate to the system settings (for example 60hz). That means it will only draw 
	// the screen 60 times a second (or higher if the system refresh rate is more than 60). However if we set vsync 
	// to false then it will draw the screen as many times a second as it can, however this can cause some visual artifacts.

	// Set the refresh rate of the back buffer.
	if(m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
	swapChainDesc.OutputWindow = hwnd;

	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Set to full screen or windowed mode.
	if(fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;
	// Now that the swap chain description has been filled out we can create it as well as the Direct3D device. The 
	// Direct3D device is very important, it is our interface to calling all of the Direct3D functions. We will use the 
	// device for almost everything from this point forward.

	// Create the swap chain and the Direct3D device.
	result = D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0, D3D10_SDK_VERSION, 
		&swapChainDesc, &m_swapChain, &m_device);
	if(FAILED(result))
	{
		return false;
	}

	// Now that we have a device and swap chain we need to get a pointer to the back buffer and then attach it to the swap chain. 
	// We'll use the CreateRenderTargetView function to attach the back buffer to our swap chain.

	// Get the pointer to the back buffer.
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&backBufferPtr);
	if(FAILED(result))
	{
		return false;
	}

	// Create the render target view with the back buffer pointer.
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if(FAILED(result))
	{
		return false;
	}

	// Release pointer to the back buffer as we no longer need it.
	backBufferPtr->Release();
	backBufferPtr = 0;
	// We will also need to set up a depth buffer description. We'll use this to create a depth buffer so that our polygons can be rendered 
	// properly in 3D space. At the same time we will attach a stencil buffer to our depth buffer. The stencil buffer can be used to achieve 
	// effects such as motion blur, volumetric shadows, and other things.

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Now we create the depth/stencil buffer using that description. You will notice we use the CreateTexture2D function to make the 
	// buffers, hence the buffer is just a 2D texture. The reason for this is that once your polygons are sorted and then rasterized they 
	// just end up being colored pixels in this 2D buffer. Then this 2D buffer is drawn to the screen.

	// Create the texture for the depth buffer using the filled out description.
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if(FAILED(result))
	{
		return false;
	}
	//Now we need to setup the depth stencil description. This allows us to control what type of depth test Direct3D will do for each pixel.

	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D10_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
	//With the description filled out we can now create a depth stencil state.

	// Create the depth stencil state.
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if(FAILED(result))
	{
		return false;
	}
	// With the created depth stencil state we can now set it on the Direct3D device so that it takes effect.

	// Set the depth stencil state on the D3D device.
	m_device->OMSetDepthStencilState(m_depthStencilState, 1);
	// The last thing we need to create is the description of the view of the depth stencil buffer. We do this so that Direct3D knows to 
	// use the depth buffer as a depth stencil texture. After filling out the description we then call the function CreateDepthStencilView 
	// via the Direct3D device to create it.

	// Initailze the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if(FAILED(result))
	{
		return false;
	}
	// With everything finally created we can now call OMSetRenderTargets. This will bind the render target view and the depth stencil 
	// buffer to the output render pipeline. This way the graphics that the pipeline renders will get drawn to our back buffer that we 
	// previously created. With the graphics written to the back buffer we can then swap it to the front and display our graphics on the user's screen.

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	m_device->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	// Now that the render targets are setup we can continue on to some extra functions that will give us more control over our scenes for 
	// future tutorials. First thing is we'll create is a rasterizer state. This will give us control over how polygons are rendered. 
	// We can do things like make our scenes render in wireframe mode or have DirectX draw both the front and back faces of polygons. 
	// By default DirectX already has a rasterizer state set up and working the exact same as the one below but you have no control 
	// to change it unless you set up one yourself.

	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D10_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D10_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if(FAILED(result))
	{
		return false;
	}

	// Now set the rasterizer state.
	m_device->RSSetState(m_rasterState);
	//The viewport also needs to be setup so that Direct3D can map clip space coordinates to the render target space. 
	//Set this to be the entire size of the window.

	// Setup the viewport for rendering.
	viewport.Width = screenWidth;
	viewport.Height = screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	// Create the viewport.
	m_device->RSSetViewports(1, &viewport);
	// Now we will create the projection matrix. The projection matrix is used to translate the 3D scene into the 2D 
	// viewport space that we previously created. We will need to keep a copy of this matrix so that we can pass 
	// it to our shaders that will be used to render our scenes.

	// Setup the projection matrix.
	fieldOfView = (float)D3DX_PI / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// Create the projection matrix for 3D rendering.
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);
	// We will also create another matrix called the world matrix. This matrix is used to convert the vertices of our objects into 
	// vertices in the 3D scene. This matrix will also be used to rotate, translate, and scale our objects in 3D space. From the start 
	// we will just initialize the matrix to the identity matrix and keep a copy of it in this object. The copy will be needed to be 
	// passed to the shaders for rendering also.

	// Initialize the world matrix to the identity matrix.
	D3DXMatrixIdentity(&m_worldMatrix);
	//This is where you would generally create a view matrix. The view matrix is used to calculate the position of where we are looking at
	// the scene from. You can think of it as a camera and you only view the scene through this camera. Because of its purpose I am going
	// to create it in a camera class in later tutorials since logically it fits better there and just skip it for now.

	// And the final thing we will setup in the Initialize function is an orthographic projection matrix. This matrix is used for rendering
	// 2D elements like user interfaces on the screen allowing us to skip the 3D rendering. You will see this used in later tutorials when
	// we look at rendering 2D graphics and fonts to the screen.

	// Create an orthographic projection matrix for 2D rendering.
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}

void D3DClass::Shutdown()
{
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if(m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}


	if(m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if(m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if(m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if(m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if(m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if(m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	if(m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	return;
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	m_device->ClearRenderTargetView(m_renderTargetView, color);
    
	// Clear the depth buffer.
	m_device->ClearDepthStencilView(m_depthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void D3DClass::EndScene()
{
	// Present the back buffer to the screen since rendering is complete.
	if(m_vsync_enabled)
	{
		// Lock to screen refresh rate.
		m_swapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		m_swapChain->Present(0, 0);
	}

	return;
}

ID3D10Device* D3DClass::GetDevice()
{
	return m_device;
}

void D3DClass::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}

void D3DClass::GetWorldMatrix(D3DXMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}

void D3DClass::GetOrthoMatrix(D3DXMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}

DXGI_MODE_DESC* D3DClass::GetDisplayModes()
{
	return displayModeList;
}