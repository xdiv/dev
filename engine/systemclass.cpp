#include "systemclass.h"


SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;
	isFullScreen = FULL_SCREEN;
	m_Sound = 0;

	m_Fps = 0;
	m_Cpu = 0;
	m_Timer = 0;
}

SystemClass::SystemClass(const SystemClass& other)
{
}

SystemClass::~SystemClass()
{
}

void SystemClass::Shutdown()
{
	// Release the graphics object.
	if(m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// Release the input object.
	if(m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	// Release the graphics object.
	if(m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// Shutdown the window.
	ShutdownWindows();

	return;
}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;


	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;

	// Initialize the windows api.
	InitializeWindows(screenWidth, screenHeight);

	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	m_Input = new InputClass;
	if(!m_Input)
	{
		return false;
	}

	// Initialization of the Input object is now different as it requires handles to the window, instance, and the screen size variables.
	// It also returns a boolean value to indicate if it was successful or not in starting Direct Input.
	// Initialize the input object.
	result = m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);
	if(!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	m_Graphics = new GraphicsClass;
	if(!m_Graphics)
	{
		return false;
	}

	// Initialize the graphics object.
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if(!result)
	{
		return false;
	}

	// Create the sound object.
	//m_Sound = new SoundClass;
	//if(!m_Sound)
	//{
	//	return false;
	//}
 //
	//// Initialize the sound object.
	//result = m_Sound->Initialize(m_hwnd);
	//if(!result)
	//{
	//	MessageBox(m_hwnd, L"Could not initialize Direct Sound.", L"Error", MB_OK);
	//	return false;
	//}

	//Create and initialize the FpsClass.
	// Create the fps object.
	m_Fps = new FpsClass;
	if(!m_Fps)
	{
		return false;
	}

	// Initialize the fps object.
	m_Fps->Initialize();

	//Create and initialize the CpuClass.
	// Create the cpu object.
	m_Cpu = new CpuClass;
	if(!m_Cpu)
	{
		return false;
	}

	// Initialize the cpu object.
	m_Cpu->Initialize();

	//Create and initialize the TimerClass.
	// Create the timer object.
	m_Timer = new TimerClass;
	if(!m_Timer)
	{
		return false;
	}

	// Initialize the timer object.
	result = m_Timer->Initialize();
	if(!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK);
		return false;
	}
	
	return true;
}


void SystemClass::Run()
{
	MSG msg;
	bool done, result;


	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));
	
	// Loop until there is a quit message from the window or the user.
	done = false;
	while(!done)
	{
		// Handle the windows messages.
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if(msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			result = Frame();
			if(!result)
			{
				done = true;
			}
		}
		
		// The check for the escape key in the Run function is now done slightly different by checking the return value of the helper 
		// function in the InputClass.

		// Check if the user pressed escape and wants to quit.
		if(m_Input->IsEscapePressed() == true)
		{
			done = true;
		}
	}

	return;
}

bool SystemClass::Frame()
{
	bool result;
	int mouseX, mouseY;
	int xDif, yDIf;

	// During the Frame function we call the Input object's own Frame function to update the states of the keyboard and mouse. This call
	// can fail so we need to check the return value.
	// Do the input frame processing.
	result = m_Input->Frame();
	if(!result)
	{
		return false;
	}

	// After the input device updates have been read we update the GraphicsClass with the location of the mouse so it can render that
	// in text on the screen.
	// Get the location of the mouse from the input object,
	m_Input->GetMouseLocation(mouseY, mouseX);
	m_Input->GetMouseLocationDif(xDif, yDIf);

	// Do the frame processing for the graphics object.
	m_Fps->Frame(); m_Timer->Frame(); m_Cpu->Frame();
	result = m_Graphics->ShowDebugInfo(mouseX, mouseY, m_Fps->GetFps(), m_Cpu->GetCpuPercentage(), m_Timer->GetTime());
	//result = m_Graphics->Frame(mouseX, mouseY);
	if(!result)
	{
		return false;
	}

	m_Graphics->RotateCamera(xDif*0.1f, yDIf*0.1f, 0.0f);
	//m_Graphics->RotateCamera(mouseX*0.1f, mouseY*0.1f, 0.0f);
	// Finally render the graphics to the screen.
	result = m_Graphics->Render();
	if(!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// Get an external pointer to this object.	
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"Engine";

	// Setup the windows class with default settings.
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = m_hinstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm       = wc.hIcon;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize        = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	//if(FULL_SCREEN)
	//replasinam musu oweride metodu
	if(isFullScreen)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;			
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth  = 1600;
		screenHeight = 900;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth)  / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, 
		// WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		WS_OVERLAPPEDWINDOW,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide the mouse cursor.
	ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if(FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	// Release the timer object.
	if(m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// Release the cpu object.
	if(m_Cpu)
	{
		m_Cpu->Shutdown();
		delete m_Cpu;
		m_Cpu = 0;
	}

	// Release the fps object.
	if(m_Fps)
	{
		delete m_Fps;
		m_Fps = 0;
	}

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
		// Check if the window is being destroyed.
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
	case WM_CLOSE:
		{
			PostQuitMessage(0);		
			return 0;
		}

	case WM_SETFOCUS:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}

		// All other messages pass to the message handler in the system class.
	default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}