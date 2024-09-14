cbuffer ConstantBufferMatrixes : register( b0 )
{
    matrix World;  
    matrix View;
    matrix Proj;
    float4 CameraPos;
    //bool RenderState;
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
    float4 PosWS : POSITION0;
    float4 PosVS : POSITION1;
    float2 Tex : TEXCOORD0;       
    float3 NormWS : NORMAL0;      
    float3 NormVS : NORMAL1;    
};

PS_INPUT VSMain( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.PosWS = mul(float4( input.Pos.xyz, 1), World );
    output.PosVS = mul( output.PosWS, View);
    output.Pos = mul( output.PosVS, Proj);
    output.NormWS = mul( float4( input.Norm, 0), World ).xyz;
    output.NormVS = mul( mul(float4( input.Norm, 0), World), View ).xyz;
    
    output.NormWS = normalize(output.NormWS);
    output.NormVS = normalize(output.NormVS);
    output.Tex = input.Tex;
    
    return output;
}
