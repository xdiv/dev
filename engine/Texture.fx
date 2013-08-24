// GLOBALS //
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
Texture2D shaderTexture;

// Another new addition to the shader is the sampler state. The sampler state allows us to modify how the pixels are written to the polygon 
// face when shaded. For example if the polygon is really far away and only makes up 8 pixels on the screen then we use the sample state to 
// figure out which pixels or what combination of pixels will actually be drawn from the original texture. The original texture may be 256 
// pixels by 256 pixels so deciding which pixels get drawn is really important to ensure that the texture still looks decent on the really
// small polygon face. I'll show you the one we will use for this shader and then explain it more:

// SAMPLE STATES //
SamplerState SampleType
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

// In the sampler state defined here we use just three elements that will be important to our shader. If you wanted more control or 
// other effects you can define it further, here is the DirectX description of sampler states:
//typedef struct D3D10_SAMPLER_DESC {
//    D3D10_FILTER Filter;
//    D3D10_TEXTURE_ADDRESS_MODE AddressU;
//    D3D10_TEXTURE_ADDRESS_MODE AddressV;
//    D3D10_TEXTURE_ADDRESS_MODE AddressW;
//    FLOAT MipLODBias;
//    UINT MaxAnisotropy;
//    D3D10_COMPARISON_FUNC ComparisonFunc;
//    FLOAT BorderColor[4];
//    FLOAT MinLOD;
//    FLOAT MaxLOD;
//} D3D10_SAMPLER_DESC;

// The most important element to us is Filter. Filter will determine how it decides which pixels will be used or combined to create the
// final look of the texture on the polygon face. In the example here I used MIN_MAG_MIP_LINEAR which is more expensive in terms of
// processing but gives the best visual result. It tells the sampler to use linear interpolation for minification, magnification, and
// mip-level sampling. I won't get into any further details in this tutorial but you can look at the D3D10_FILTER description on MSDN to
// see the other options available to achieve different effects. AddressU and AddressV are set to Wrap which ensures that the coordinates
// stay between 0.0f and 1.0f. Anything outside of that wraps around and is placed between 0.0f and 1.0f.
// The VertexInputType and PixelInputType have changed as we are no longer using color in our vertex type and have instead moved to using 
// texture coordinates. Texture coordinates take a U and V float coordinate so we use float2 as its type. The TEXCOORD[N] semantic used
// here allows you define the texture coordinate set number. In the shader we have just one texture coordinate set so we use the value
// zero as it is the first and only texture coordinate set.

// TYPEDEFS //
// dadëjus float4 color : COLOR; galima paduoti ir spalvà vëlau sujoininti(sumuoti)
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

// Vertex Shader
PixelInputType TextureVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	// The only difference in this vertex shader in comparison to the color vertex shader from the previous tutorial is that we take a copy
	// of the texture coordinates instead of the color value that was used previously.
	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
    
    return output;
}

// The pixel shader has changed. We will now use the sample function that is associated with the shader texture and give it the sampler 
// state we defined above and the texture coordinates for this pixel. It will then process that and return the resulting sampled 
// pixel from the texture that should be drawn.
// Pixel Shader
float4 TexturePixelShader(PixelInputType input) : SV_Target
{
    float4 textureColor;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);

	// papildþius PixelInputType VertexInputType
	// turetu dadëti pasirinkta splvà prie texturos, teoriðkai
	// textureColor += input.color

    return textureColor;
}

technique10 TextureTechnique
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, TextureVertexShader()));
        SetPixelShader(CompileShader(ps_4_0, TexturePixelShader()));
        SetGeometryShader(NULL);
    }
}