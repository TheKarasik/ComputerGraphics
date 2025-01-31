////////////////////////////////////////////////////////////////////////////////
// Filename: reflection.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    float4 camPos;
};

cbuffer ClipPlaneBuffer : register(b1)
{
    float4 clipPlane;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    /*float4 color : COLOR;*/
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    /*float4 color : COLOR;*/
    float clip : SV_ClipDistance0;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType VSMain(VertexInputType input)
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
    
    // Calculate the normal vector against the world matrix only and then normalize the final value.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

    // Calculate the tangent vector against the world matrix only and then normalize the final value.
    output.tangent = mul(input.tangent, (float3x3)worldMatrix);
    output.tangent = normalize(output.tangent);

    // Calculate the binormal vector against the world matrix only and then normalize the final value.
    output.binormal = mul(input.binormal, (float3x3)worldMatrix);
    output.binormal = normalize(output.binormal);

    // Send the color map color into the pixel shader.	
    /*output.color = input.color;*/

    // Set the clipping plane.
    output.clip = dot(mul(input.position, worldMatrix), clipPlane);

    return output;
}

////////////////////////////////////////////////////////////////////////////////
// Filename: reflection.ps
////////////////////////////////////////////////////////////////////////////////


//////////////
// TEXTURES //
//////////////
Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);


//////////////
// SAMPLERS //
//////////////
SamplerState SampleType : register(s0);


//////////////////////
// CONSTANT BUFFERS //
//////////////////////
cbuffer LightBuffer : register(b2)
{
    //float4 lightDiffuseColor;
    float4 lightDirection;
    //float colorTextureBrightness;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 PSMain(PixelInputType input) : SV_TARGET
{
    float3 lightDir;
    float4 textureColor;
    float4 bumpMap;
    float3 bumpNormal;
    float lightIntensity;
    float4 color;


    // Invert the light direction for calculations.
    lightDir = -lightDirection;

    // Sample the color texture.
    textureColor = colorTexture.Sample(SampleType, input.tex);

    // Combine the color map value into the texture color.
    float colorTextureBrightness = 2.0f;
    textureColor = saturate(/*input.color * */textureColor * colorTextureBrightness);

    // Calculate the bump map using the normal map.
    bumpMap = normalTexture.Sample(SampleType, input.tex);
    bumpMap = (bumpMap * 2.0f) - 1.0f;
    bumpNormal = input.normal + bumpMap.x * input.tangent + bumpMap.y * input.binormal;
    bumpNormal = normalize(bumpNormal);
    lightIntensity = saturate(dot(bumpNormal, lightDir));
    
    // Calculate the first bump mapped pixel color.
    //color = saturate(lightDiffuseColor * lightIntensity);
    color = lightIntensity * textureColor;

    return color;
}