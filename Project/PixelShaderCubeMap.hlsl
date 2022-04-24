struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float4 wPosition : W_POSITION;
    float4 normal : NORMAL;
    float2 uv : UV;
    float4 lightPosition : LIGHTPOS;
};


TextureCube tex : register(t3);

SamplerState samp1 : register(s0);

float4 main(PixelShaderInput input) : SV_TARGET
{
    //float4 test = float4(0.0f, 1.0f, 0.0f, 0.0f);
    //return test;
   return tex.Sample(samp1, float3(input.uv, 1.0f));
}