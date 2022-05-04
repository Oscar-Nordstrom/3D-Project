struct pixleInput {
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
};

struct PixelShaderOutput
{
	float4 color : SV_Target2;
};

Texture2D<float4> tex : register(t0);

SamplerState samp : register(s0);

PixelShaderOutput main(pixleInput input)
{
	PixelShaderOutput output;
	output.color = tex.Sample(samp, input.uv);
	//output.color = float4(1.0f, 0.0f, 0.0f, 0.0f);
	return output;
}