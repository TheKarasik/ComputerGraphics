cbuffer ConstantBufferMatrixes : register( b0 )
{
    matrix WorldProjView;
}

struct VS_INPUT                   
{
    float4 Pos : POSITION;      
};

struct PS_INPUT                   
{
    float4 Pos : SV_POSITION;    
    float4 PosDepth : TEXTURE0;  
};

PS_INPUT VSMain( VS_INPUT input )
{
    PS_INPUT output;
    input.Pos.w = 1.0f;
    output.Pos = mul(input.Pos, WorldProjView);
    // Пишем позицию в depthPosition
    output.PosDepth = output.Pos;
	
    return output;
}

float4 PSMain( PS_INPUT input) : SV_Target
{
    float depthValue = input.PosDepth.z / input.PosDepth.w;

    return float4(depthValue, depthValue, depthValue, 1.0f);
}