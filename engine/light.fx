matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
Texture2D shaderTexture;

// The first difference between the texture shader and the new light shader is the addition of two new global variables that hold the diffuse
// color and the direction of the light. Before this shader is called these two variables will be set from values in the new LightClass.

float4 ambientColor;
float4 diffuseColor;
float3 lightDirection;

// SAMPLE STATES //
SamplerState SampleType
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

// Both structures now have a 3 float normal vector. The normal vector is used for calculating the amount of light by using the angle 
// between the direction of the normal and the direction of the light.

// TYPEDEFS //
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

// Vertex Shader
PixelInputType LightVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
	//The normal vector for this vertex is calculated in world space and then normalized before being sent as input into the pixel shader.

    // Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);

    return output;
}

// Pixel Shader
float4 LightPixelShader(PixelInputType input) : SV_Target
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;


    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);

	// Set the default output color to the ambient light value for all pixels.
    color = ambientColor;

	// This is where the lighting equation that was discussed earlier is now implemented. The light intensity value is calculated as 
	// the dot product between the normal vector of triangle and the light direction vector.

    // Invert the light direction for calculations.
    lightDir = -lightDirection;

    // Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(input.normal, lightDir));

	if(lightIntensity > 0.0f)
    {
        // Determine the final diffuse color based on the diffuse color and the amount of light intensity.
        color += (diffuseColor * lightIntensity);
    }

	// Saturate the final light color.
    color = saturate(color);

	//And finally the diffuse value of the light is combined with the texture pixel value to produce the color result.
    // Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
    //color = saturate(diffuseColor * lightIntensity);

    // Multiply the texture pixel and the final diffuse color to get the final pixel color result.
    color = color * textureColor;
	

    return color;
}

//The technique stays the same. The only thing that changes is the name of it and the pixel and vertex shader names.
// Technique
technique10 LightTechnique
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, LightVertexShader()));
        SetPixelShader(CompileShader(ps_4_0, LightPixelShader()));
        SetGeometryShader(NULL);
    }
}