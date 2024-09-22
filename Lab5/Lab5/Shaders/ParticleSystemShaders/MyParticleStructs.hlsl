struct Particle
{
    float4 pos; 
    float4 velocity;
    //float4 acceleration;
    float4 color;
    float4 colorDelta;
    
    float lifeSpan;
    float age;
    float weight;
    float weightDelta;
    float size;
    float sizeDelta;
};

struct Attractor
{
    float4 position;
    float gravity;
    float mass;
    float killZoneRadius;
};

cbuffer emitterConstantBuffer : register(b4)
{
    float4 emitterPosition;
    float4 emitterScale;
    float4 emitterPartitioning; //don't know how to name that but influence repartition inside the volume (making cool effects)
    float4 color;
    float4x4 emitterRotation;
    
    uint emitterMaxSpawn;
    float particlesBaseSpeed;
    float particlesLifeSpan;
    float particlesWeightStart;
    float particlesWeightFinish;
    float particleSizeStart;
    float particleSizeFinish;
    float rng_seed;
};

struct ParticleIndexElement
{
    float distance;
    float index;
};