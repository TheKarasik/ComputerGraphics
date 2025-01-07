#include "MyParticleStructs.hlsl"

cbuffer aliveListCountConstantBuffer : register(b3)
{
    uint nbAliveParticles;
};

cbuffer sceneConstantBuffer : register(b0)
{
    float4x4 view;
    float4x4 proj;
}

StructuredBuffer<Particle> particleList : register(t0);
StructuredBuffer<ParticleIndexElement> aliveParticlesIndex : register(t1);

struct GeometryShaderInput
{
    float4 Position : SV_POSITION;
    float3 oPosition : TEXCOORD0;
    float4 Color : TEXCOORD1;
    //float4 Normal : TEXCOORD2;
    float3 velocity : TEXCOORD3;
    //uint   orientation : TEXCOORD4;
    float  size : TEXCOORD5;
    float age : TEXCOORD6;
};

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float3 oPosition : TEXCOORD0;
    float4 Color : TEXCOORD1;
    //float4 Normal : TEXCOORD2;
    float3 center : TEXCOORD3;
    float  radius : TEXCOORD4;
    float2 uv : TEXCOORD5;
    float age : TEXCOORD6;
};

struct PixelShaderOutput
{
    float4 color : SV_Target;
};

GeometryShaderInput VSMain(uint vertexId : SV_VertexID)
{
    GeometryShaderInput output;
    
    uint index = aliveParticlesIndex[nbAliveParticles - vertexId - 1].index;
    
    Particle p = particleList[index];

    output.Position = p.pos;
    output.oPosition = p.pos.xyz;
    output.velocity = p.velocity.xyz;

    //float alpha = smoothstep(0.0, abs(p.lifeSpan), p.age);
    float alpha = smoothstep(abs(p.lifeSpan), 0.0, p.age);
    output.Color = p.color;
    output.Color.a *= alpha;
    //output.Normal = p.normal;

    //output.orientation = p.orientation;
    //output.size = p.sizeStart + alpha * (p.sizeEnd - p.sizeStart);
    output.size = p.size + alpha * p.sizeDelta;

    output.age = p.age;
    
    return output;
}

[maxvertexcount(4)]
void GSMain(point GeometryShaderInput input[1], inout TriangleStream<PixelShaderInput> OutStream)
{
    //discard if no particle
    if (input[0].Color.a <= 0.0)
        return;

    PixelShaderInput output;
    output.Color = input[0].Color;
    //output.Normal = input[0].Normal;

    float3 pos = input[0].oPosition;
    float particleSize = input[0].size;

    output.center = input[0].oPosition;
    output.radius = particleSize;

    float3 right = float3(1.0, 0.0, 0.0);
    float3 up = float3(0.0, 1.0, 0.0);
    
     //Camera Plane
    right = view._m00_m10_m20;
    up = view._m01_m11_m21;

    float4x4 viewProj = mul(view, proj);

    //in counterClockwise and right handed coordinate
    // 1    3
    // |  / |
    // | /  |
    // 2    4

    // Upper left vertex
    output.oPosition = pos + particleSize * (-right + up);
    output.Position = mul(float4(output.oPosition, 1.0), viewProj);
    output.uv = float2(0.0, 0.0);
    output.age = input[0].age;
    OutStream.Append(output);

    // Bottom left vertex
    output.oPosition = pos + particleSize * (-right - up);
    output.Position = mul(float4(output.oPosition, 1.0), viewProj);
    output.uv = float2(0.0, 1.0);
    output.age = input[0].age;
    OutStream.Append(output);

    // Upper right vertex
    output.oPosition = pos + particleSize * (right + up);
    output.Position = mul(float4(output.oPosition, 1.0), viewProj);
    output.uv = float2(1.0, 0.0);
    output.age = input[0].age;
    OutStream.Append(output);

    // Bottom right vertex
    output.oPosition = pos + particleSize * (right - up);
    output.Position = mul(float4(output.oPosition, 1.0), viewProj);
    output.uv = float2(1.0, 1.0);
    output.age = input[0].age;
    OutStream.Append(output);

    OutStream.RestartStrip();

}

PixelShaderOutput PSMain(PixelShaderInput input)
{
    PixelShaderOutput output;

    output.color = input.Color;

    float len = length(input.oPosition - input.center);
    output.color.a = output.color.a * (1.0 - smoothstep(0.0, input.radius, len));
    if (output.color.a < 0.1) output.color.a = 0.0f;
    
    return output;

}