matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
Texture2D shaderTexture;

// The first difference between the texture shader and the new light shader is the addition of two new global variables that hold the diffuse
// color and the direction of the light. Before this shader is called these two variables will be set from values in the new LightClass.

float4 ambientColor;
float4 diffuseColor;
float3 lightDirection;

float3 cameraPosition;
float4 specularColor;
float specularPower;
float4 extracolor;

// SAMPLE STATES //
SamplerState SampleType
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

BlendState AlphaBlendingState
{
    BlendEnable[0] = false;
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
	float3 viewDirection : TEXCOORD1;
};

// Vertex Shader
PixelInputType LightVertexShader(VertexInputType input)
{
    PixelInputType output;
	float4 worldPosition;
    
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

	//The viewing direction is calculated here in the vertex shader. We calculate the world position of the vertex and subtract that from 
	//the camera position to determine where we are viewing the scene from. The final value is normalized and sent into the pixel shader.
	// Calculate the position of the vertex in the world.
    worldPosition = mul(input.position, worldMatrix);

    // Determine the viewing direction based on the position of the camera and the position of the vertex in the world.
    output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
	
    // Normalize the viewing direction vector.
    output.viewDirection = normalize(output.viewDirection);

    return output;
}

// Pixel Shader
float4 LightPixelShader(PixelInputType input) : SV_Target
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;
	float3 reflection;
    float4 specular;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);

	textureColor = textureColor * extracolor * 2.0f;

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

		 // Saturate the ambient and diffuse color.
        color = saturate(color);

		// Calculate the reflection vector based on the light intensity, normal vector, and light direction.
        reflection = normalize(2 * lightIntensity * input.normal - lightDir); 

		// Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
        specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
    }

    // Multiply the texture pixel and the final diffuse color to get the final pixel color result.
    color = color * textureColor;
	
	// Add the specular component last to the output color.
    color = saturate(color + specular);

    return color;
}

//The technique stays the same. The only thing that changes is the name of it and the pixel and vertex shader names.
// Technique
technique10 LightTechnique
{
    pass pass0
    {
		SetBlendState(AlphaBlendingState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_4_0, LightVertexShader()));
        SetPixelShader(CompileShader(ps_4_0, LightPixelShader()));
        SetGeometryShader(NULL);
    }
}