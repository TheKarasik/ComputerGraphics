#include "MyParticleStructs.hlsl"

static uint rng_state;

uint wang_hash(uint seed)
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

uint rand_xorshift()
{
    // Xorshift algorithm from George Marsaglia's paper
    rng_state ^= (rng_state << 13);
    rng_state ^= (rng_state >> 17);
    rng_state ^= (rng_state << 5);
    return rng_state;
}

float rand_xorshift_normalized()
{
    return float(rand_xorshift()) * (1.0 / 4294967296.0);
}



ConsumeStructuredBuffer<uint> DeadList : register(u0);
RWStructuredBuffer<Particle> ParticleList : register(u1);
AppendStructuredBuffer<ParticleIndexElement> AliveParticleIndex : register(u2);
RWBuffer<uint> IndirectDispatchArgs : register(u3);

cbuffer DeadListCB : register(b2)
{
    uint dead_particles;
}

[numthreads(256,1,1)]
void CSMain(uint3 id : SV_DispatchThreadID)
{
    if (id.x < dead_particles && id.x < emitterMaxSpawn)
    {
        rng_state = wang_hash(id.x + rng_seed);
        Particle p = (Particle) 0;
        p.pos = emitterPosition;
        float3 position = emitterPartitioning.xyz * float3(rand_xorshift_normalized() - 0.5, rand_xorshift_normalized() - 0.5, rand_xorshift_normalized() - 0.5);
        position = mul(emitterScale.xyz * normalize(position), emitterRotation);
        p.pos.xyz += position;
        p.pos.w = 1.0f;

        p.velocity = particlesBaseSpeed * normalize(float4(position - emitterPosition, 0.0f));

        p.lifeSpan = particlesLifeSpan * rand_xorshift_normalized();
        p.age = abs(p.lifeSpan);
        p.weight = particlesWeightStart;
        p.weightDelta = (particlesWeightFinish - particlesWeightStart) / p.lifeSpan;

        //p.orientation = particleOrientation;
        p.color = color;
        p.size = particleSizeStart;
        p.sizeDelta = (particleSizeFinish - particleSizeStart) / p.lifeSpan;

        uint index = DeadList.Consume();
        ParticleList[index] = p;

        ParticleIndexElement pe;
        pe.index = index;
        pe.distance = 0;
        AliveParticleIndex.Append(pe);

        InterlockedAdd(IndirectDispatchArgs[0], 1);
    }
}