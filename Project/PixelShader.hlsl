struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float4 wPosition : W_POSITION;
    float4 normal : NORMAL;
    float2 uv : UV;
    float4 lightPosition : LIGHTPOS;
};

struct PixelShaderOutput
{
    float4 position : SV_Target0;
    float4 wPosition : SV_Target1;
    float4 color : SV_Target2;
    float4 normal : SV_Target3;
    float4 ambient : SV_Target4;
    float4 specular : SV_Target5;
};

Texture2D<float4> tex1 : register(t0); //Diffuse
Texture2D<float4> tex2 : register(t1); //Specular
Texture2D<float4> tex3 : register(t2); //Ambient
SamplerState samp1 : register(s0);

Texture2D<float4> shadowMap : register(t3);
//RWTexture2D<float4> backbuffer : register(u0);


PixelShaderOutput main(PixelShaderInput input)
{
    
   /* //Perspective divide to project the texture coords onto the shadow map
    input.lightPosition.xy /= input.lightPosition.w; //Now in NDC
    
    //lightPosition.xy is in [-1,1], but to sample the shadow map we need [0,1], so scale by 1/2 and add 1/2
    float2 smTex = float2(0.5f * input.lightPosition.x + 0.5f, -0.5f * input.lightPosition.y + 0.5f);
    
    //Compute pixel depth for shadowing
    float depth = input.lightPosition.z / input.lightPosition.w;
    
    float w, h;
    shadowMap.GetDimensions(w, h);
    float SAMP_SIZE = w;
    
    float dx = 1 / SAMP_SIZE;
    float s0 = (shadowMap.Sample(samp1, smTex + float2(0.0f, 0.0f)).r < depth) ? 0.0f : 1.0f;
    float s1 = (shadowMap.Sample(samp1, smTex + float2(dx,   0.0f)).r < depth) ? 0.0f : 1.0f;
    float s2 = (shadowMap.Sample(samp1, smTex + float2(0.0f,   dx)).r < depth) ? 0.0f : 1.0f;
    float s3 = (shadowMap.Sample(samp1, smTex + float2(dx,     dx)).r < depth) ? 0.0f : 1.0f;

    float2 texelPos = smTex * SAMP_SIZE;
    
    float2 lerps = frac(texelPos);
    
    float ShadowCoeff = lerp(lerp(s0,s1,lerps.x), lerp(s2, s3, lerps.x), lerps.y);*/
    
    
    
   /* input.lightPosition.xyz /= input.lightPosition.w; //Now in NDC
    
    float shadowC = 1.0f;
    
    if (shadowMap.Sample(samp1, input.lightPosition.xy).r > input.lightPosition.z)
    {
        shadowC = 0.0f;
    }*/

    input.lightPosition.xy /= input.lightPosition.w; //Now in NDC
    float2 smTex = float2(0.5f * input.lightPosition.x + 0.5f, -0.5f * input.lightPosition.y + 0.5f);

    //Sample
    //Compare points


    PixelShaderOutput output;
    
    output.color = tex1.Sample(samp1, input.uv);// * shadowC;
    output.position = input.position;
    output.wPosition = input.wPosition;
    output.normal = input.normal;
    output.specular = tex2.Sample(samp1, input.uv);// * shadowC;
    output.ambient = tex3.Sample(samp1, input.uv);
    
 
    
    return output;


}