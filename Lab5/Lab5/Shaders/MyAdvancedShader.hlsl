Texture2D txDiffuse : register( t0 );

SamplerState samLinear : register( s0 );

cbuffer ConstantBufferMatrixes : register( b0 )
{
    matrix World;  
    matrix ProjView;
    float4 CameraPos;
    bool IsPerspective;
}

struct Material
{
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

cbuffer ConstantBufferMaterial : register(b1)
{
    Material material;
}
struct LightData
{        
    matrix mViewProj;
    float4 directionWS;
    float4 color;
};

cbuffer ConstantBufferMaterial : register(b2)
{
    LightData light_data;
}

struct VS_INPUT                   
{
    float4 Pos : POSITION;        
    float2 Tex : TEXCOORD0;       
    float3 Norm : NORMAL;         
};

struct PS_INPUT                   
{
    float4 Pos : SV_POSITION;    
    float4 PosWS : POSITION;     
    float2 Tex : TEXCOORD0;       
    float3 Norm : NORMAL;    
};

float4 DoDiffuse(LightData light, float4 L, float4 N)
{
    const float NdotL = max(dot(L, N), 0);
    return light.color * NdotL;
}

float4 DoSpecular(LightData light, Material mat, float4 V, float4 L, float4 N)
{
    float4 R = normalize(reflect(-L, N));
    const float RdotV = max(dot(R, V), 0);
    return light.color * pow(RdotV, mat.specular_power);
}

LightingResult DoDirectionalLight(LightData light, Material mat, float4 V, float4 N)
{
    LightingResult result;
    //float4 L = normalize(-light.directionVS);
    float4 L = normalize(-light.directionWS);

    result.diffuse = DoDiffuse(light, L, N);
    result.specular = DoSpecular(light, mat, V, L, N);

    return result;
}

PS_INPUT VSMain( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.PosWS = mul(float4( input.Pos.xyz, 1), World );
    output.Pos = mul( output.PosWS, ProjView);
    output.Norm = mul( float4( input.Norm, 0), World ).xyz;
    output.Norm = normalize(output.Norm);
    output.Tex = input.Tex;
    
    return output;
}

float4 PSMain( PS_INPUT input) : SV_Target
{
    //return float4 (input.Norm, 1);
    /*for(int i=0; i<2; i++)
    {
        finalColor += saturate( dot( (float3)vLightDir[i], input.Norm) * vLightColor[i] );
    }*/
    float4 diffVal = txDiffuse.Sample( samLinear, input.Tex );
    //diffVal.a = 1.0f;
    
    clip(diffVal.a - 0.01f);
    
    float4 ambient = diffVal * material.global_ambient;
    float4 diffuse = diffVal;
    float4 specular;
    ambient *= diffVal;
    float4 N = normalize(float4(input.Norm, 0.0f));

    LightingResult lit;
    //light_data.color = 1;
    //light_data.directionWS = float4(0,-1,0,1);
    
    //float shadow = 1.0f;
    /*if (Lights[0].type == DIRECTIONAL_LIGHT)
    {
        float4 shadowPosH = mul(input.posWS, mul(Lights[0].mViewProj, mT));
        shadow = CalcShadowFactor(ShadCompSamp, ShadowMap, shadowPosH);
    }*/
    lit = DoDirectionalLight(light_data, material,  normalize(CameraPos - input.PosWS), N);
    diffuse *= lit.diffuse;
    specular = lit.specular;
    specular *= material.specular_scale;
    float4 col = float4((ambient + diffuse + specular).rgb, 1);

    if(!IsPerspective) col = 0.2989 * col.x + 0.5870 * col.y + 0.1140 * col.z;

    return col;
}

/*float4 PSSolid( PS_INPUT input) : SV_Target
{
    return vOutputColor;
}*/