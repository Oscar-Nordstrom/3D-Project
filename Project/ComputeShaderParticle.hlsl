
cbuffer cb : register(b3)
{
    float dt;//delta time
    float time;//total time
}
RWBuffer<float> particles : register(u1);

[numthreads(10, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    float3 currentPosition = float3(particles[DTid.x * 3], particles[DTid.x * 3 + 1], particles[DTid.x * 3 + 2]);
    float3 nextPosition = currentPosition;

    nextPosition.y = particles[DTid.x * 3 + 1] - 3.0f * dt;
    if (nextPosition.y < -50) {
        nextPosition.y = 50.0f;
    }

    particles[DTid.x * 3 + 1] = nextPosition.y;
}