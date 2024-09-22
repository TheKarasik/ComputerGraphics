cbuffer initIndirectComputeArgs1DConstantBuffer : register(b4)
{
    float ThreadGroupX;
}

RWBuffer<uint> indirectComputeArgs : register(u0);

[numthreads(1,1,1)]
void CSMain(uint3 id : SV_DispatchThreadID)
{
    indirectComputeArgs[0] = (uint) ceil(indirectComputeArgs[0] / ThreadGroupX);
    indirectComputeArgs[1] = 1;
    indirectComputeArgs[2] = 1;
}