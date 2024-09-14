struct VS_INPUT                   
{
    float4 Pos : POSITION;      
};

struct PS_INPUT                   
{
    float4 Pos : SV_POSITION;    
};

struct GS_IN
{
    float4 pos : POSITION;
};

struct GS_OUT
{
    float4 pos : SV_POSITION;
    uint arrInd : SV_RenderTargetArrayIndex;
};

struct CascadeData
{
    matrix ViewProj[4];
    float4 Distances;
};
cbuffer ConstantBufferMatrixes : register( b0 )
{
    matrix World;  
}

cbuffer ConstantBufferMatrixes : register( b1 )
{
    CascadeData cascade_data; 
}

GS_IN VSMain( VS_INPUT input )
{
    GS_IN output;
    input.Pos.w = 1.0f;
    output.pos = mul(float4(input.Pos.xyz, 1), World);
	
    return output;
}

[instance(4)]
[maxvertexcount(3)]
void GSMain(triangle GS_IN p[3], in uint id : SV_GSInstanceID, inout TriangleStream<GS_OUT> stream)
{
    [unroll]
    for (int i = 0; i < 3; ++i)
    {
        GS_OUT gs = (GS_OUT) 0;
        gs.pos = mul(float4(p[i].pos.xyz, 1.0f), cascade_data.ViewProj[id]);
        gs.arrInd = id;
        stream.Append(gs);
    }
}