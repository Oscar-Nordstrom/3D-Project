struct pixleInput {
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
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

Texture2D<float4> tex : register(t0);

SamplerState samp : register(s0);

PixelShaderOutput main(pixleInput input)
{
	PixelShaderOutput output;
	output.position = input.position;
	output.wPosition = input.position;
	output.color = tex.Sample(samp, input.uv);
	output.normal = float4(input.normal, 0.0f);
	output.ambient = float4(1.0f, 1.0f, 1.0f, 1.0f);
	output.specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	return output;
}