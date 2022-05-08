struct VsInput {
	float3 position : POSITION;
};
struct VsOutput {
	float4 position : SV_POSITION;
	float4x4 view : VIEW;
	float4x4 proj : PROJ;
};

cbuffer cBuf : register(b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;
};

VsOutput main(VsInput input)
{
	VsOutput output;
	output.position = mul(world, float4(input.position, 1.0f));
	//output.position = float4(input.position, 1.0f);
	output.view = view;
	output.proj = projection;
	return output;
}