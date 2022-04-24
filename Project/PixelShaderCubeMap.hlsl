struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float4 wPosition : W_POSITION;
    float4 normal : NORMAL;
    float2 uv : UV;
    float4 lightPosition : LIGHTPOS;
};

Texture2D<float4> tex1 : register(t0); //Diffuse
Texture2D<float4> tex2 : register(t1); //Specular
Texture2D<float4> tex3 : register(t2); //Ambient
TextureCube cMap : register(t3);

SamplerState samp1 : register(s0);

cbuffer camcb : register(b2)
{
    float4 camPos;
}

float4 main(PixelShaderInput input) : SV_TARGET
{
    
    float3 viewVector = input.wPosition.xyz - camPos.xyz;
    float3 reflectVec = reflect(viewVector, normalize(input.normal.xyz));
    
    float4 dtex = cMap.Sample(samp1, reflectVec);
    dtex.w = 1; //we force it
    return dtex;
    
    //float4 test = float4(0.0f, 1.0f, 0.0f, 0.0f);
    //return test;
   // return cMap.Sample(samp1, float3(input.uv, 1.0f));
}