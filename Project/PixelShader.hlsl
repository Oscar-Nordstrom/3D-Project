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



PixelShaderOutput main(PixelShaderInput input)
{
    
 
    float shadowCoeff = 1.0f;
    //Convert to NDC
    input.lightPosition.xy /= input.lightPosition.w;//OK
    //Translate to UV (0-1)
    float2 smTex = float2(input.lightPosition.x * 0.5f + 0.5f, input.lightPosition.y * (-0.5f) + 0.5f); //OK
    //Compute pixel depth for shadowing
    float depth = input.lightPosition.z / input.lightPosition.w;//OK
    //Sample
    float4 sampled = shadowMap.Sample(samp1, smTex);//Maybe need another sampler?
    //Check if shadowd
    if (sampled.r < depth)//Is this right?
    {
        //If the light sees is less than what we see, there is a shadow
        shadowCoeff = 0.0f;
    }
    
    

  

    PixelShaderOutput output;
    
    output.color = tex1.Sample(samp1, input.uv);// * shadowCoeff;
    output.specular = tex2.Sample(samp1, input.uv);// * shadowCoeff;
    output.ambient = tex3.Sample(samp1, input.uv);
    output.position = input.position;
    output.wPosition = input.wPosition;
    output.normal = input.normal;
    
 
    
    return output;


}