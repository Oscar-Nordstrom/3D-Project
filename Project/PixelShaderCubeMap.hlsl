struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float4 wPosition : W_POSITION;
    float4 normal : NORMAL;
    float2 uv : UV;
    float4 lightPosition : LIGHTPOS;
};

TextureCube tex : register(t0);

SamplerState samp1 : register(s0);

float4 main(PixelShaderInput input) : SV_TARGET
{
   return tex.Sample(samp1, float3(input.uv, 1.0f));
}