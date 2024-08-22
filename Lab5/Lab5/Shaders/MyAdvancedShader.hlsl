Texture2D txDiffuse : register( t0 );
Texture2D txShadowmap : register( t1 );

SamplerState samLinear : register( s0 );
SamplerState samShadowmap : register( s1 );

/*
 * RenderState == 0 - Рендер для игрока (MainRender);
 * RenderState == 1 - Рендер теней (Shadowmap);
 * RenderState == 2 - Рендер мини карты (Minimap);
 */

cbuffer ConstantBufferMatrixes : register( b0 )
{
    matrix World;  
    matrix ProjView;
    float4 CameraPos;
    //bool RenderState;
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
    float4 position;
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
    float4 col;
    float4 diffVal = txDiffuse.Sample( samLinear, input.Tex );
    diffVal.a = 1.0f;
    
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
    col = float4((ambient + specular).rgb, 1);
    //col = float4((ambient + diffuse + specular).rgb, 1);

    //Добавляем тени
    float bias = 0.001f;
    float2 projectTexCoord;
    float4 lightViewPosition = mul(input.PosWS,light_data.mViewProj);
    projectTexCoord.x =  lightViewPosition.x / lightViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -lightViewPosition.y / lightViewPosition.w / 2.0f + 0.5f;
    if((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        float depthValue = txShadowmap.Sample(samShadowmap, projectTexCoord).r;
        float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
        lightDepthValue = lightDepthValue - bias;
        if(lightDepthValue < depthValue)
        {
            float3 lightPos = light_data.position - input.PosWS;
            lightPos = normalize(lightPos);
            float lightIntensity = saturate(dot(input.Norm, lightPos));
            if(lightIntensity > 0.0f)
            {
                //float4 difcol = float4(1.0f, 1.0f, 1.0f, 1.0f);;
                col += (diffuse * lightIntensity);
                //col = saturate(col);
            }
        }
    }
    
    //col += float4((ambient + diffuse + specular).rgb, 1);
    
    // = mul( input.PosWS, mViewProj);
    /*float2 projectTexCoord;
    projectTexCoord.x =  input.Pos.x / input.Pos.w / 2.0f + 0.5f;
    projectTexCoord.y = -input.Pos.y / input.Pos.w / 2.0f + 0.5f;
    if(saturate(projectTexCoord.x) == projectTexCoord.x && saturate(projectTexCoord.y) == projectTexCoord.y)
    {
        float depthValue = txShadowmap.Sample(samShadowmap, projectTexCoord).r;
        float lightDepthValue = input.Pos.z
    }*/
    
    //Обработка мини карты
    //if(RenderState == 2) col = 0.2989 * col.x + 0.5870 * col.y + 0.1140 * col.z;
    
    return col;
    
}
