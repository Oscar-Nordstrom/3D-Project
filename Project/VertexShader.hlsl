cbuffer cBuf : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 projection;
};
cbuffer cBuf : register(b1)
{
    float4x4 lightView;
    float4x4 lightProjection;
};

struct VertexShaderInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
};

struct VSOut
{
    float4 position : SV_POSITION;
    float4 wPosition : W_POSITION;
    float4 normal : NORMAL;
    float2 uv : UV;
    float4 lightPosition : LIGHTPOS;
};

VSOut main(VertexShaderInput input)
{
    float4 position = float4(input.position, 1.0f);
    float4 wPosition = float4(input.position, 1.0f);
    float4 normal = float4(input.normal, 0.0f);
    
    VSOut output;
    
   wPosition = mul(world, wPosition); //World space
    
    position = mul(world, position);//World space
    //position = mul(view, position);//View space
    //position = mul(projection, position);//Clip space

    
    normal = mul(world, normal); //World space 
    
    //Light
    output.lightPosition = wPosition;
   // output.lightPosition = mul(lightView, wPosition);
   // output.lightPosition = mul(lightProjection, output.lightPosition);
    
    output.position = position;
    output.wPosition = wPosition;
    output.normal = normal;
    output.uv = input.uv;
   
	return output;
}