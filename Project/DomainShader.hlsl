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


struct DS_OUTPUT
{
	//float4 vPosition  : SV_POSITION;
	float4 position : SV_POSITION;
	float4 wPosition : W_POSITION;
	float4 normal : NORMAL;
	float2 uv : UV;
	float4 lightPosition : LIGHTPOS;
};

struct HS_CONTROL_POINT_OUTPUT
{
	//float3 vPosition : WORLDPOS; 
	float4 position : SV_POSITION;
	float4 wPosition : W_POSITION;
	float4 normal : NORMAL;
	float2 uv : UV;
	float4 lightPosition : LIGHTPOS;
};

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor;
	float InsideTessFactor : SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 3

float3 ProjToPlane(float3 tripoint, float3 normal, float3 UVW)
{
	normal = normalize(normal);
	float3 vec = UVW - tripoint;
	float3 dist = dot(vec, normal);
	return UVW - dist * normal;
}

[domain("tri")]
DS_OUTPUT main(HS_CONSTANT_DATA_OUTPUT input, float3 UVW : SV_DomainLocation, const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> tri)
{
	DS_OUTPUT Output = (DS_OUTPUT)0;


	float3 pos = UVW.x * tri[0].position.xyz + UVW.y * tri[1].position.xyz + UVW.z * tri[2].position.xyz;
	float3 p0 = ProjToPlane(tri[0].position.xyz, tri[0].normal.xyz, pos);
	float3 p1 = ProjToPlane(tri[1].position.xyz, tri[1].normal.xyz, pos);
	float3 p2 = ProjToPlane(tri[2].position.xyz, tri[2].normal.xyz, pos);
	float3x3 mat = float3x3(p0, p1, p2);
	float alpha = 0.75f;
	float3 finalPos = (1 - alpha) * pos + mul((alpha * UVW), mat);

	//finalPos = pos;


	//view and projection transformations
	Output.position = mul(view, float4(finalPos, 1.0f));
	Output.position = mul(projection, Output.position);
	Output.wPosition = float4(finalPos, 1.0f);



	float3 finalNormal = UVW.x * tri[0].normal + UVW.y * tri[1].normal + UVW.z * tri[2].normal;
	Output.normal = mul(view, float4(finalNormal, 0.0f));
	Output.normal = mul(projection, Output.normal);

	float2 finalTex = UVW.x * tri[0].uv + UVW.y * tri[1].uv + UVW.z * tri[2].uv;
	Output.uv = finalTex;

	float3 finalLightPos = UVW.x * tri[0].lightPosition + UVW.y * tri[1].lightPosition + UVW.z * tri[2].lightPosition;
	Output.lightPosition = mul(lightView, float4(finalLightPos, 1.0f));
	Output.lightPosition = mul(lightProjection, Output.lightPosition);

	return Output;
}
