////////////////////////////////////////////////////////////////////////////////
// Filename: water.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	float4 cameraPosition;
};

cbuffer CamNormBuffer : register(b1)
{
	matrix reflectionMatrix;
	//float padding1;
	float2 normalMapTiling;
	//float2 padding2;
};

SamplerState SampleType : register(s0);
Texture2D refractionTexture : register(t0);
Texture2D reflectionTexture : register(t1);
Texture2D normalTexture : register(t2);

cbuffer WaterBuffer : register(b2)
{
	float4 refractionTint;
	float4 lightDirection;
	float waterTranslation;
	float reflectRefractScale;
	float specularShininess;
	float padding;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
	float4 reflectionPosition : TEXCOORD0;
    float4 refractionPosition : TEXCOORD1;
	float3 viewDirection : TEXCOORD2;
    float2 tex1 : TEXCOORD3;
    float2 tex2 : TEXCOORD4;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType VSMain(VertexInputType input)
{
    PixelInputType output;
    matrix reflectProjectWorld;
	matrix viewProjectWorld;
	float4 worldPosition;

    
	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Create the reflection projection world matrix.
    reflectProjectWorld = mul(reflectionMatrix, projectionMatrix);
    reflectProjectWorld = mul(worldMatrix, reflectProjectWorld);

	// Calculate the input position against the reflectProjectWorld matrix.
    output.reflectionPosition = mul(input.position, reflectProjectWorld);

	// Create the view projection world matrix for refraction.
    viewProjectWorld = mul(viewMatrix, projectionMatrix);
    viewProjectWorld = mul(worldMatrix, viewProjectWorld);
   
	// Calculate the input position against the viewProjectWorld matrix.
    output.refractionPosition = mul(input.position, viewProjectWorld);
	
	 // Calculate the position of the vertex in the world.
    worldPosition = mul(input.position, worldMatrix);

    // Determine the viewing direction based on the position of the camera and the position of the vertex in the world.
    output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

    // Normalize the viewing direction vector.
    output.viewDirection = normalize(output.viewDirection);

	// Create two different texture sample coordinates for tiling the water normal map over the water quad multiple times.
    output.tex1 = input.tex / normalMapTiling.x;
    output.tex2 = input.tex / normalMapTiling.y;

	return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 PSMain(PixelInputType input) : SV_TARGET
{
	float4 normalMap1;
	float4 normalMap2;
	float3 normal1;
	float3 normal2;
	float3 normal;
	float2 refractTexCoord;
	float2 reflectTexCoord;
	float4 reflectionColor;
	float4 refractionColor;
	float3 heightView;
	float r;
	float fresnelFactor;
	float4 color;
	float3 reflection;
	float specular;


	// Move the position the water normal is sampled from to simulate moving water.	
	input.tex1.y += waterTranslation;
	input.tex2.y += waterTranslation;

	// Sample the normal from the normal map texture using the two different tiled and translated coordinates.
	normalMap1 = normalTexture.Sample(SampleType, input.tex1);
	normalMap2 = normalTexture.Sample(SampleType, input.tex2);
	
	// Expand the range of the normal from (0,1) to (-1,+1).
	normal1 = (normalMap1.rgb * 2.0f) - 1.0f;
	normal2 = (normalMap2.rgb * 2.0f) - 1.0f;

	// Combine the normals to add the normal maps together.
	normal = normalize(normal1 + normal2);
		
	// Calculate the projected refraction texture coordinates.
    refractTexCoord.x = input.refractionPosition.x / input.refractionPosition.w / 2.0f + 0.5f;
    refractTexCoord.y = -input.refractionPosition.y / input.refractionPosition.w / 2.0f + 0.5f;
	
	// Calculate the projected reflection texture coordinates.
    reflectTexCoord.x = input.reflectionPosition.x / input.reflectionPosition.w / 2.0f + 0.5f;
    reflectTexCoord.y = -input.reflectionPosition.y / input.reflectionPosition.w / 2.0f + 0.5f;
		
	// Re-position the texture coordinate sampling position by the scaled normal map value to simulate the rippling wave effect.
	reflectTexCoord = reflectTexCoord + (normal.xy * reflectRefractScale);
	refractTexCoord = refractTexCoord + (normal.xy * reflectRefractScale);
	
	// Sample the texture pixels from the textures using the updated texture coordinates.
    reflectionColor = reflectionTexture.Sample(SampleType, reflectTexCoord);
    refractionColor = refractionTexture.Sample(SampleType, refractTexCoord);

	// Combine the tint with the refraction color.
	refractionColor = saturate(refractionColor * refractionTint);

	// Get a modified viewing direction of the camera that only takes into account height.
	heightView.x = input.viewDirection.y;
	heightView.y = input.viewDirection.y;
	heightView.z = input.viewDirection.y;

	// Now calculate the fresnel term based solely on height.
	r = (1.2f - 1.0f) / (1.2f + 1.0f);
	fresnelFactor = max(0.0f, min(1.0f, r + (1.0f - r) * pow(1.0f - dot(normal, heightView), 2)));

	// Combine the reflection and refraction results for the final color using the fresnel factor.
	//color = lerp(reflectionColor, refractionColor, fresnelFactor);
	color = lerp(reflectionColor, refractionColor, 0.6f);

	// Calculate the reflection vector using the normal and the direction of the light.
	reflection = -reflect(normalize(lightDirection.xyz), normal);
	
	// Calculate the specular light based on the reflection and the camera position.
	specular = dot(normalize(reflection), normalize(input.viewDirection));

	// Check to make sure the specular was positive so we aren't adding black spots to the water.
	if(specular > 0.0f)
	{
		// Increase the specular light by the shininess value.
		specular = pow(specular, specularShininess);

		// Add the specular to the final color.
		color = saturate(color + specular);
	}

	return color;
	//return reflectionColor;
}