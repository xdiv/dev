#include "colorshaderclass.h"

// As usual the class constructor initializes all the private pointers in the class to null.
ColorShaderClass::ColorShaderClass()
{
	m_effect = 0;
	m_technique = 0;
	m_layout = 0;

	m_worldMatrixPtr = 0;
	m_viewMatrixPtr = 0;
	m_projectionMatrixPtr = 0;
}


ColorShaderClass::ColorShaderClass(const ColorShaderClass& other)
{
}


ColorShaderClass::~ColorShaderClass()
{
}

// The Initialize function will call the initialization function for the shader. We pass in the name of the HLSL 
// shader file, in this tutorial it is named color.fx.//
bool ColorShaderClass::Initialize(ID3D10Device* device, HWND hwnd)
{
	bool result;


	// Initialize the shader that will be used to draw the triangle.
	result = InitializeShader(device, hwnd, L"../Engine/color.fx");
	if(!result)
	{
		return false;
	}

	return true;
}

//The Shutdown function will call the shutdown of the shader.
void ColorShaderClass::Shutdown()
{
	// Shutdown the shader effect.
	ShutdownShader();

	return;
}

// Render will first set the parameters inside the shader using the SetShaderParameters function. Once the parameters 
// are set it then calls RenderShader to draw the green triangle using the HLSL shader.
void ColorShaderClass::Render(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	// Set the shader parameters that it will use for rendering.
	SetShaderParameters(worldMatrix, viewMatrix, projectionMatrix);

	// Now render the prepared buffers with the shader.
	RenderShader(device, indexCount);

	return;
}

// Now we will start with one of the more important functions to this tutorial which is called InitializeShader. This function is what 
// actually loads the shader file and makes it usable to DirectX and the GPU. You will also see the setup of the layout and how the 
// vertex buffer data is going to look on the graphics pipeline in the GPU. The layout will need the match the VertexType in 
// the modelclass.h file as well as the one defined in the color.fx file.
bool ColorShaderClass::InitializeShader(ID3D10Device* device, HWND hwnd, WCHAR* filename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	D3D10_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D10_PASS_DESC passDesc;


	// Initialize the error message.
	errorMessage = 0;
	// Here is where we compile the shader program into an effect. We give it the name of the shader file, the shader version 
	// (4.0 in DirectX 10), and the effect to compile the shader into. If it fails compiling the shader it will put an error message inside 
	// the errorMessage string which we send to another function to write out the error. If it still fails and there is no errorMessage 
	// string then it means it could not find the shader file in which case we pop up a dialog box saying so.

	// Load the shader in from the file.
	result = D3DX10CreateEffectFromFile(filename, NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, 
					    device, NULL, NULL, &m_effect, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, filename);
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, filename, L"Missing Shader File", MB_OK);
		}

		return false;
	}
	// Once the shader code has successfully compiled into an effect we then use that effect to get the technique inside the shader. 
	// We will use the technique to draw with the shader from this point forward.

	// Get a pointer to the technique inside the shader.
	m_technique = m_effect->GetTechniqueByName("ColorTechnique");
	if(!m_technique)
	{
		return false;
	}
	
	// The next step is to create the layout of the vertex data that will be processed by the shader. As this shader uses a position and 
	// color vector we need to create both in the layout specifying the size of both. The semantic name is the first thing to fill out 
	// in the layout, this allows the shader to determine the usage of this element of the layout. As we have two different elements we 
	// use POSITION for the first one and COLOR for the second. The next important part of the layout is the Format. For the position 
	// vector we use DXGI_FORMAT_R32G32B32_FLOAT and for the color we use DXGI_FORMAT_R32G32B32A32_FLOAT. The final thing you need to pay 
	// attention to is the AlignedByteOffset which indicates how the data is spaced in the buffer. For this layout we are telling it the 
	// first 12 bytes are position and the next 16 bytes will be color, AlignedByteOffset shows where each element begins. 
	// You can use D3D10_APPEND_ALIGNED_ELEMENT instead of placing your own values in AlignedByteOffset and it will figure out the spacing 
	// for you. The other settings I've made default for now as they are not needed in this tutorial.

	// Now setup the layout of the data that goes into the shader.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;
	//Once the layout description has been setup we can get the size of it and then create the input layout using the D3D device.

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Get the description of the first pass described in the shader technique.
	m_technique->GetPassByIndex(0)->GetDesc(&passDesc);

	// Create the input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, 
					   &m_layout);
	if(FAILED(result))
	{
		return false;
	}

	//We will also grab pointers to the global matrices that are inside the shader file. This way when we set a matrix from the 
	//GraphicsClass inside the shader easily by just using these pointers.

	// Get pointers to the three matrices inside the shader so we can update them from this class.
	m_worldMatrixPtr = m_effect->GetVariableByName("worldMatrix")->AsMatrix();
	m_viewMatrixPtr = m_effect->GetVariableByName("viewMatrix")->AsMatrix();
	m_projectionMatrixPtr = m_effect->GetVariableByName("projectionMatrix")->AsMatrix();

	return true;
}

//ShutdownShader releases the interfaces and pointers that were setup in the InitializeShader function.
void ColorShaderClass::ShutdownShader()
{
	// Release the pointers to the matrices inside the shader.
	m_worldMatrixPtr = 0;
	m_viewMatrixPtr = 0;
	m_projectionMatrixPtr = 0;

	// Release the pointer to the shader layout.
	if(m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pointer to the shader technique.
	m_technique = 0;

	// Release the pointer to the shader.
	if(m_effect)
	{
		m_effect->Release();
		m_effect = 0;
	}

	return;
}

//The OutputShaderErrorMessage writes out error messages that are generating when compiling either vertex shaders or pixel shaders.
void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for(i=0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

// The SetShaderVariables function exists to make setting the global variables in the shader easier. The matrices used in this function 
// are created inside the GraphicsClass, after which this function is called to send them from there into the shader during the Render function call.
void ColorShaderClass::SetShaderParameters(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	// Set the world matrix variable inside the shader.
	m_worldMatrixPtr->SetMatrix((float*)&worldMatrix);

	// Set the view matrix variable inside the shader.
	m_viewMatrixPtr->SetMatrix((float*)&viewMatrix);

	// Set the projection matrix variable inside the shader.
	m_projectionMatrixPtr->SetMatrix((float*)&projectionMatrix);

	return;
}


// RenderShader is the second function called from the ColorShaderClass::Render function. SetShaderParameters is called before this to 
// ensure the shader parameters are setup correctly before invoking the HLSL program. RenderShader will invoke the color.fx HLSL shader
// program through the technique pointer.
// The first step in this function is to set our input layout to active in the input assembler. This lets the GPU know the format of the 
// data in the vertex buffer. The second step is to get the description of the technique from the shader. The technique is what tells the 
// GPU which pixel and vertex shader functions to call to draw the vertex buffer. In this case we are getting the description of 
// ColorTechnique from our color.fx file. Once we have the technique we then loop through each pass in the technique and render the 
// triangle by calling the DrawIndexed DirectX function using the D3D device. Note that the shader only has a single pass currently (pass0) 
// but I have set up the render function to handle multiple passes for future tutorials. Once this function is called it will render the green triangle.
void ColorShaderClass::RenderShader(ID3D10Device* device, int indexCount)
{
	D3D10_TECHNIQUE_DESC techniqueDesc;
	unsigned int i;
	

	// Set the input layout.
	device->IASetInputLayout(m_layout);

	// Get the description structure of the technique from inside the shader so it can be used for rendering.
	m_technique->GetDesc(&techniqueDesc);

	// Go through each pass in the technique (should be just one currently) and render the triangles.
	for(i=0; i<techniqueDesc.Passes; ++i)
	{
		m_technique->GetPassByIndex(i)->Apply(0);
		device->DrawIndexed(indexCount, 0, 0);
	}

	return;
}