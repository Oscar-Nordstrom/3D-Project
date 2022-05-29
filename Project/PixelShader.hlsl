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

cbuffer lightcb : register(b1)
{
    float4 lightColor;
    float3 lightDir;
}

Texture2D<float4> map_Kd : register(t0); //Diffuse
Texture2D<float4> map_Ks : register(t1); //Specular
Texture2D<float4> map_Ka : register(t2); //Ambient
SamplerState samp1 : register(s0);
SamplerState shadowSamp : register(s1);

//Texture2D<float4> shadowMap : register(t3);
Texture2DArray<float4> shadowMaps : register(t3);


PixelShaderOutput main(PixelShaderInput input)
{
    
    bool lit = false;
    int numLights = 1;
    float shadowCoeff = 1.0f;
    for (int i = 0; i < numLights; i++) {
        if (lit) {
            break;
        }
        if (dot(normalize(lightDir), input.normal) < 0.0f) {
            continue;
        }
        //Convert to NDC
        input.lightPosition.xy /= input.lightPosition.w;//OK
        //Translate to UV (0-1)
        float2 smTex = float2(input.lightPosition.x * 0.5f + 0.5f, input.lightPosition.y * (-0.5f) + 0.5f);//OK
        //Compute pixel depth for shadowing
        float depth = input.lightPosition.z / input.lightPosition.w;
        //Sample
        float4 sampled = shadowMaps.SampleLevel(shadowSamp, float3(smTex, i), 0);
        //Check if shadowd
        if (sampled.r < depth)
        {
            //If the light sees is less than what we see, there is a shadow
            shadowCoeff = 0.0f;
        }
        else {
            lit = true;
        }
    }
  

    PixelShaderOutput output;
    
    output.color = map_Kd.Sample(samp1, input.uv) * shadowCoeff;
    output.specular = map_Ks.Sample(samp1, input.uv) * shadowCoeff;
    output.ambient = map_Ka.Sample(samp1, input.uv);
    output.position = input.position;
    output.wPosition = input.wPosition;
    output.normal = input.normal;
    
 
    
    return output;


}