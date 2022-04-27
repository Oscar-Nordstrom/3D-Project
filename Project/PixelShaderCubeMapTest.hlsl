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

cbuffer camcb : register(b2)
{
    float4 camPos;
}

Texture2D<float4> tex1 : register(t0); //Diffuse
Texture2D<float4> tex2 : register(t1); //Specular
Texture2D<float4> tex3 : register(t2); //Ambient
TextureCube cMap : register(t3);//Cube map
SamplerState samp1 : register(s0);
SamplerState shadowSamp : register(s1);

Texture2D<float4> shadowMap : register(t3);



PixelShaderOutput main(PixelShaderInput input)
{
    
 
    float shadowCoeff = 1.0f;
    //Convert to NDC
    input.lightPosition.xy /= input.lightPosition.w; //OK
    //Translate to UV (0-1)
    float2 smTex = float2(input.lightPosition.x * 0.5f + 0.5f, input.lightPosition.y * (-0.5f) + 0.5f); //OK
    //Compute pixel depth for shadowing
    float depth = input.lightPosition.z / input.lightPosition.w; //OK
    //Sample
    float4 sampled = shadowMap.Sample(shadowSamp, smTex); //Maybe need another sampler?
    //Check if shadowd
    if (sampled.r < depth)//Is this right?
    {
        //If the light sees is less than what we see, there is a shadow
        shadowCoeff = 0.0f;
    }
    

    
    
    float4 red = float4(1.0f, 0.0f, 0.0f, 0.0f);


    float3 viewVector = input.wPosition.xyz - camPos.xyz;
    //float3 reflectVec = reflect(viewVector, normalize(input.normal.xyz));

   // float4 dtex = cMap.Sample(samp1, reflectVec);
    float4 dtex = cMap.Sample(samp1, viewVector);
    dtex.w = 1; //we force it
  

    PixelShaderOutput output;
    
    //output.color = cMap.Sample(samp1, float3(input.uv, 1.0f)); // * shadowCoeff;
    output.color = dtex;
    //output.color = red;
    output.specular = tex2.Sample(samp1, input.uv); // * shadowCoeff;
    output.ambient = tex3.Sample(samp1, input.uv);
    output.position = input.position;
    output.wPosition = input.wPosition;
    output.normal = input.normal;
    
 
    
    return output;


}