#include "MyVertexShader.hlsl"

#define SPOT_LIGHT 0
#define POINT_LIGHT 1
#define DIRECTIONAL_LIGHT 2

struct Light
{        
    float4 positionWS;
    float4 directionWS;
    float4 positionVS;
    float4 directionVS;
    float4 color;
    float SpotlightAngle;
    float Range;
    float Intensity;
    unsigned int Type;
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


Texture2D txDiffuse : register( t0 );
Texture2D txNormal : register( t1 );
Texture2D txDepth : register(t2);
Texture2D txSpecular : register(t3);

//StructuredBuffer<Light> Lights : register( t4 );

cbuffer ScreenToViewParams : register( b0 )
{
    float4x4 InverseProjection;
    float2 ScreenDimensions;
}

cbuffer LightBuffer : register( b1 )
{
    Light light;
}

/*cbuffer LightIndexBuffer : register( b1 )
{
    uint LightIndex;
}*/

/*cbuffer Material : register( b2 )
{
    Material Mat;
}*/

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

float DoAttenuation( Light light, float d )
{
    return 1.0f - smoothstep( light.Range * 0.75f, light.Range, d );
}

float DoSpotCone( Light light, float4 L )
{
    float minCos = cos( radians( light.SpotlightAngle ) );
    
    float maxCos = lerp( minCos, 1, 0.5f );
    float cosAngle = dot(light.directionVS , -L );
    
    return smoothstep( minCos, maxCos, cosAngle );
}

float4 DoDiffuse( Light light, float4 L, float4 N )
{
    float NdotL = max( dot( N, L ), 0 );
    return light.color * NdotL;
}

float4 DoSpecular( Light light, Material material, float4 V, float4 L, float4 N )
{
    float4 R = normalize( reflect( -L, N ) );
    float RdotV = max( dot( R, V ), 0 );

    return light.color * pow( RdotV, material.specular_power );
}


LightingResult DoPointLight( Light light, Material mat, float4 V, float4 P, float4 N )
{
    LightingResult result;
    float4 L = light.positionVS - P;
    //float4 L = P - light.positionVS;
    float distance = length( L );
    L = L / distance;

    float attenuation = DoAttenuation( light, distance );

    result.diffuse = DoDiffuse( light, L, N ) * attenuation * light.Intensity;
    result.specular = DoSpecular( light, mat, V, L, N ) * attenuation * light.Intensity;

    return result;
}

LightingResult DoDirectionalLight( Light light, Material mat, float4 V, float4 P, float4 N )
{
    LightingResult result;
    
    float4 L = normalize( -light.directionVS);

    result.diffuse = DoDiffuse( light, L, N ) * light.Intensity;
    result.specular = DoSpecular( light, mat, V, L, N ) * light.Intensity;

    return result;
}

LightingResult DoSpotLight( Light light, Material mat, float4 V, float4 P, float4 N )
{
    LightingResult result;
    
    float4 L = light.positionVS - P;
    float distance = length( L );
    L = L / distance;

    float attenuation = DoAttenuation( light, distance );
    float spotIntensity = DoSpotCone( light, L );

    result.diffuse = DoDiffuse( light, L, N ) * attenuation * spotIntensity * light.Intensity;
    result.specular = DoSpecular( light, mat, V, L, N ) * attenuation * spotIntensity * light.Intensity;

    return result;
}

[earlydepthstencil]
float4 PSMain(PS_INPUT input) : SV_Target
{ 
    float4 eyePos = {0, 0, 0, 1};

    int2 Coord = input.Pos.xy;
    float depth = txDepth.Load(int3(Coord, 0)).r;
    float4 diffuse = txDiffuse.Load(int3(Coord, 0));
    float4 specular = txSpecular.Load( int3( Coord, 0 ) );
    float specularPower = exp2( specular.a * 10.5f );

    float4 P = ScreenToView(float4(Coord, depth, 1));
    //float4 P = ScreenToWorld(float4(Coord, depth, 1));
    float4 V = normalize(eyePos - P);
    float4 N = txNormal.Load(int3(Coord,0));

    LightingResult lit = (LightingResult)0;

    Material Mat = (Material)0;
    Mat.specular_power = specularPower;

    switch ( light.Type )
    {
    case DIRECTIONAL_LIGHT:
        lit = DoDirectionalLight( light, Mat, V, P, N );
        break;
    case POINT_LIGHT:
        lit = DoPointLight( light, Mat, V, P, N );
        break;
    case SPOT_LIGHT:
        lit = DoSpotLight( light, Mat, V, P, N );
        break;
    }

    return ( diffuse * lit.diffuse ) + ( Mat.specular_scale * lit.specular );
}