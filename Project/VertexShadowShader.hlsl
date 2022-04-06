cbuffer cBuf : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 projection;
};

struct VertexShaderInput
{
    float3 position : POSITION;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    output.position = mul(world, float4(input.position, 1.0f));
    output.position = mul(view, output.position);
    output.position = mul(projection, output.position);
    
    return output;
}