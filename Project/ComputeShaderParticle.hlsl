
Texture2D<float4> colors : register(t0);

RWTexture2D<unorm float4> backbuffer : register(u0);

[numthreads(20, 20, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    int2 texPos = int2(0, 0);
    texPos.x = DTid.x;
    texPos.y = DTid.y;
    int3 tPos = int3(texPos, 0);

    float4 matColor = colors.Load(tPos);

    backbuffer[DTid.xy] = matColor;
}