//Global variables inside the shader allow us to have external access to modify the internal shader variables. For example with these 
//three matrices we will set them in the GraphicsClass and then pass them into this shader so they can be used by the ColorVertexShader 
//below. You can use many types of variables such as int or float and then set them externally for the shader program to use.
// GLOBALS //
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;

// TYPEDEFS //
struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};


// The vertex shader is called by the GPU when it is processing data from the vertex buffers that have been sent to it. This function which 
// I named ColorVertexShader will be called for every single vertex in the vertex buffer. The input to the vertex shader must match the data
// format in the vertex buffer as well as the type definition in the shader source file which in this case is VertexInputType. 
// The output of the vertex shader will be sent to the pixel shader. In this case the output type is called PixelInputType which is defined above as well.

// With that in mind you see that the vertex shader creates an output variable that is of the PixelInputType type. It then takes the 
// position of the input vertex and multiplies it by the world, view, and then projection matrices. This will place the vertex in the 
// correct location for rendering in 3D space according to our view. After that the output variable takes a copy of the input color and 
// then returns the output which will be used as input to the pixel shader. Also note that I do set the W value of the input position to 
// 1.0 otherwise it is undefined since we only read in a XYZ vector for position.

// Vertex Shader
PixelInputType ColorVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the input color for the pixel shader to use.
    output.color = input.color;
    
    return output;
}

// The pixel shader draws each pixel on the polygons that will be rendered to the screen. In this pixel shader it uses PixelInputType as 
// input and returns a float4 as output which represents the final pixel color. In this pixel shader we just tell it to color the pixel 
// the same as the input value of the color. Note that the pixel shader gets its input from the vertex shader output.

// Pixel Shader
float4 ColorPixelShader(PixelInputType input) : SV_Target
{
    return input.color;
}

// The technique is the actual "shader". This is what is used to render our polygons and call the vertex and pixel shaders, you can consider 
// it as the main() function of the HLSL shader. You can set it up to do multiple passes and call all sorts of different pixel and vertex 
// shaders to create a desired effect. In this example we will only use a single pass and only call the vertex and pixel shader we defined 
// earlier. We won't be calling a geometry shader either as it isn't required for our purposes right now.

// One thing to note is that we set our vertex shader to version 4.0 by using the vs_4_0 as the first argument in SetVertexShader. 
// This gives us access to the DirectX 10 HLSL functionality that corresponds to vertex shader 4.0. As well we also set the pixel shader 
// to version 4.0 using ps_4_0 in the SetPixelShader function. You can set this to be older vertex and pixel shader versions but we want 
// to work with the latest and greatest.

// Technique
technique10 ColorTechnique
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, ColorVertexShader()));
        SetPixelShader(CompileShader(ps_4_0, ColorPixelShader()));
        SetGeometryShader(NULL);
    }
}