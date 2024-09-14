cbuffer ConstantBufferMatrixes : register( b0 )
{
    matrix World;  
    matrix View;
    matrix Proj;
    float4 CameraPos;
}

struct VS_INPUT                   
{
    float4 Pos : POSITION;      
};

struct PS_INPUT                   
{
    float4 Pos : SV_POSITION;     
};

PS_INPUT VSMain( VS_INPUT input )
{
    PS_INPUT output;
    input.Pos.w = 1.0f;
    output.Pos = mul(float4(input.Pos.xyz, 1), World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Proj);
    // Пишем позицию в depthPosition
	
    return output;
}
