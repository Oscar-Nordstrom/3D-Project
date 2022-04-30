struct VsInput {
	float3 position : POSITION;
};
struct VsOutput {
	float4 position : SV_POSITION;
};

cbuffer cBuf : register(b0)
{
	float4x4 world;
	float4x4 view;//Not needed
	float4x4 projection;//Not needed
};

VsOutput main(VsInput input)
{
	VsOutput output;
	float4 wPosition = float4(input.position, 1.0f);
	output.position = mul(world, wPosition);
	return output;
}