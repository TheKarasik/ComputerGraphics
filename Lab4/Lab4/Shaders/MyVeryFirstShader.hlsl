struct VS_IN
{
	float4 pos : POSITION0;
	float4 col : COLOR0;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
 	float4 col : COLOR;
};

cbuffer MatrixConstantBuffer : register(b0)
{
	float4x4 transform;
}

/*cbuffer MatrixConstantBuffer : register(b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;
}*/

PS_IN VSMain( VS_IN input )
{
	PS_IN output = (PS_IN)0;
	
	output.pos = mul(input.pos, transform);

	/*output.pos = mul(input.pos, world);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);*/
	
	output.col = input.col;
	
	return output;
}

float4 PSMain( PS_IN input ) : SV_Target
{
	float4 col = input.col;
//#ifdef TEST
//	if (input.pos.x > 400) col = TCOLOR;
//#endif
	return col;
}