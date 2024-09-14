#include "MyVertexShader.hlsl"

struct DirectionalLight
{
    float4x4 mView;
    float4x4 mProj;
    float4 positionWS;
    float4 directionWS;
    float4 directionVS;
    float4 color;
    float Intensity;
};

struct CascadeData
{
    matrix ViewProj[4];
    /*matrix View[4];
    matrix Proj[4];*/
    float4 Distances;
};

Texture2D txDiffuse : register( t0 );
Texture2D txShadowmap : register( t1 );
Texture2DArray txCascadeShadowmaps : register( t2 );

SamplerState samLinear : register( s0 );
SamplerState samShadowmap : register( s1 );

struct PS_OUTPUT
{
    float4 Diffuse              : SV_Target0;
    float4 Normal               : SV_Target1;
    float4 Specular             : SV_Target2;
    float4 LightAccumulation    : SV_Target3;
};

struct Material
{
    //float4 ambient_color;
    float4 global_ambient;
    float4 specular_color;
    float specular_power;
    float specular_scale;
    float2 padding;
};

struct LightingResult
{
    float4 diffuse;
    float4 specular;
};

cbuffer Material : register( b0 )
{
    Material Mat;
};

cbuffer directional_light : register( b1 )
{
    DirectionalLight dir_light;
}

cbuffer ScreenToViewParams : register( b2 )
{
    float4x4 InverseProjection;
    float2 ScreenDimensions;
}

cbuffer Cascade : register (b3)
{
    CascadeData cd;
}

float4 ClipToView( float4 clip )
{
    float4 view = mul( clip, InverseProjection );
    view = view / view.w;

    return view;
}

float4 ScreenToView( float4 screen )
{
    float2 texCoord = screen.xy / ScreenDimensions;

    float4 clip = float4( float2( texCoord.x, 1.0f - texCoord.y ) * 2.0f - 1.0f, screen.z, screen.w );

    return ClipToView( clip );
}

float4 DoDiffuse( DirectionalLight light, float4 L, float4 N )
{
    float NdotL = max( dot( N, L ), 0 );
    return light.color * NdotL;
}

float4 DoSpecular( DirectionalLight light, Material material, float4 V, float4 L, float4 N )
{
    float4 R = normalize( reflect( -L, N ) );
    float RdotV = max( dot( R, V ), 0 );

    return light.color * pow( RdotV, material.specular_power );
}

LightingResult DoDirectionalLight( DirectionalLight light, Material mat, float4 V, float4 N )
{
    LightingResult result;
    
    float4 L = normalize( -light.directionVS);

    result.diffuse = DoDiffuse( light, L, N ) * light.Intensity;
    result.specular = DoSpecular( light, mat, V, L, N ) * light.Intensity;

    return result;
}

[earlydepthstencil]
PS_OUTPUT PSMain(PS_INPUT input)
{
    PS_OUTPUT OUT = (PS_OUTPUT)0;
    OUT.Diffuse = txDiffuse.Sample(samLinear, input.Tex);
    OUT.Diffuse.a = 1.0f;
    
    OUT.Normal = normalize(float4(input.NormVS, 0));
    
    OUT.Specular = float4(Mat.specular_color.rgb, log2(Mat.specular_power) / 10.5f);

    float4 eyePos = {0, 0, 0, 1};
    
    float4 P = ScreenToView(input.Pos);
    float4 V = normalize(eyePos - P);
    float4 N = OUT.Normal;
    
    LightingResult lit = DoDirectionalLight( dir_light, Mat, V, N );
    

    OUT.LightAccumulation = OUT.Diffuse * Mat.global_ambient + OUT.Specular * lit.specular;
    
    //Тени от Directional Light
    int layer = 3;
    float viewDepthVal = abs(input.PosVS.z);
    float dists[4] = {cd.Distances.x, cd.Distances.y, cd.Distances.z, cd.Distances.w };
    for (int i = 0; i < 4; i++)
    {
        if (viewDepthVal < dists[i].x)
        {
            layer = i;
            break;
        }
    }
    
    float bias = 0.001f;
    float2 projectTexCoord;
    float4 lightViewPosition = mul(input.PosWS, cd.ViewProj[layer]);
    projectTexCoord.x =  lightViewPosition.x / lightViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -lightViewPosition.y / lightViewPosition.w / 2.0f + 0.5f;
    if((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        float depthValue =  txCascadeShadowmaps.Sample(samShadowmap,float3(projectTexCoord, layer)).r;
        float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
        lightDepthValue = lightDepthValue - bias;
        if(lightDepthValue < depthValue )
        {
            float lightIntensity = saturate(dot(input.NormWS, -dir_light.directionWS));
            if(lightIntensity > 0.0f)
            {
                OUT.LightAccumulation += OUT.Diffuse * lit.diffuse * lightIntensity;
            }
        }
    }
    return OUT;
}