#include "MyParticleStructs.hlsl"

RWBuffer<uint> indirectDrawArgs : register(u0);
AppendStructuredBuffer<ParticleIndexElement> aliveParticleIndex: register(u1);
AppendStructuredBuffer<uint> deadParticleIndex : register(u2);
RWStructuredBuffer<Particle> particleList : register(u3);
ConsumeStructuredBuffer<ParticleIndexElement> aliveParticleIndexIn: register(u4);
RWBuffer<uint> indirectDispatchArgs : register(u5);

StructuredBuffer<Attractor> attractorBuffer : register(t0);

cbuffer simulate_cb : register(b0)
{
    float3 camPosition;
    uint attractorsNum;
    float dt;
    float water_level;
}

[numthreads(256,1,1)]
void CSMain(uint3 id : SV_DispatchThreadID, uint groupID : SV_GroupIndex)
{
    if (id.x == 0)
    {
        indirectDrawArgs[0] = 0;
        indirectDrawArgs[1] = 1;
        indirectDrawArgs[2] = 0;
        indirectDrawArgs[3] = 0;
        indirectDrawArgs[4] = 0;

        indirectDispatchArgs[0] = 0;
        indirectDispatchArgs[1] = 1;
        indirectDispatchArgs[2] = 1;
    }

    GroupMemoryBarrierWithGroupSync();

    ParticleIndexElement particle = aliveParticleIndexIn.Consume();
    Particle p = particleList[particle.index];

    if(p.age > 0)
    {
        if (p.lifeSpan >= 0)
        {
            p.age -= dt;
        }

        float4 particleForce = 0;

        for (uint i = 0; i < attractorsNum; ++i)
        {
            Attractor a = attractorBuffer[i];
            float4 direction = a.position - p.pos;
            float distance = length(direction);

            particleForce += normalize(direction) * (a.gravity * p.weight * a.mass) / (max(1.0, distance * distance));
        }

        float3 acceleration = particleForce.xyz / p.weight;
        p.velocity.xyz += acceleration * dt;
        p.pos.xyz += p.velocity.xyz * dt;
        if (p.pos.y <= water_level)
        {
            p.age = 0.0;
        }

        for (uint j = 0; j < attractorsNum; ++j)
        {
            Attractor a = attractorBuffer[j];
            float distance = length(a.position - p.pos);
            if (distance < a.killZoneRadius)
            {
                p.age = 0.0;
            }
        }
        
        p.weight += p.weightDelta;
        p.size += p.sizeDelta;

        if (p.age > 0)
        {
            particle.distance = length(p.pos - float4(camPosition, 1));
            aliveParticleIndex.Append(particle);

            InterlockedAdd(indirectDrawArgs[0], 1);
            InterlockedAdd(indirectDispatchArgs[0], 1);
            
        }
        else
        {
            p.age = -1.0;
            deadParticleIndex.Append(particle.index);
        }
    }

    particleList[particle.index] = p;
}