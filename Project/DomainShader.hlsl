cbuffer cBuf : register(b0)//Matrices
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;
};
cbuffer cBuf : register(b1)//Directional Light
{
	float4x4 lightView1;
	float4x4 lightProjection1;
};
cbuffer cBuf2 : register(b2)//Spot light 1
{
	float4x4 lightView2;
	float4x4 lightProjection2;
};
cbuffer cBuf3 : register(b3)//Spot light 2
{
	float4x4 lightView3;
	float4x4 lightProjection3;
};
cbuffer cBuf4 : register(b4)//Spot light 3
{
	float4x4 lightView4;
	float4x4 lightProjection4;
};

struct DS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 wPosition : W_POSITION;
	float4 normal : NORMAL;
	float2 uv : UV;
	float4 lightPosition1 : LIGHTPOS1;//Directional Light
	float4 lightPosition2 : LIGHTPOS2;//Spot Light 1
	float4 lightPosition3 : LIGHTPOS3;//Spot Light 2
	float4 lightPosition4 : LIGHTPOS4;//Spot Light 3
};

struct HS_CONTROL_POINT_OUTPUT
{
	float4 position : SV_POSITION;
	float4 wPosition : W_POSITION;
	float4 normal : NORMAL;
	float2 uv : UV;
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
	float3 vec = UVW - tripoint;//Vector from corener to position in triangle
	float3 dist = dot(vec, normal);//Lenght of vector in normals direction
	return UVW - dist * normal;//Returns the new position of the position
}

[domain("tri")]
/*											      Barycentric coordinates			        Control points from hull shader	*/
DS_OUTPUT main(HS_CONSTANT_DATA_OUTPUT input, float3 UVW : SV_DomainLocation, const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> tri)
{
	DS_OUTPUT Output = (DS_OUTPUT)0;


	float3 pos = UVW.x * tri[0].position.xyz + UVW.y * tri[1].position.xyz + UVW.z * tri[2].position.xyz;//Gives us a position inide the triangle
	/*Phong tesselation*/
	float3 p0 = ProjToPlane(tri[0].position.xyz, tri[0].normal.xyz, pos);
	float3 p1 = ProjToPlane(tri[1].position.xyz, tri[1].normal.xyz, pos);
	float3 p2 = ProjToPlane(tri[2].position.xyz, tri[2].normal.xyz, pos);
	float3x3 mat = float3x3(p0, p1, p2);
	float alpha = 0.5f;
	float3 phongPoint = mul(UVW, mat);
	float3 finalPos = ((1 - alpha) * pos) + (alpha * phongPoint);


	//view and projection transformations
	Output.position = mul(view, float4(finalPos, 1.0f));
	Output.position = mul(projection, Output.position);
	Output.wPosition = float4(finalPos, 1.0f);



	float3 finalNormal = UVW.x * tri[0].normal.xyz + UVW.y * tri[1].normal.xyz + UVW.z * tri[2].normal.xyz;//New normal
	Output.normal = float4(finalNormal.xyz, 0.0f);


	float2 finalTex = UVW.x * tri[0].uv + UVW.y * tri[1].uv + UVW.z * tri[2].uv;//New UVW coordinates
	Output.uv = finalTex;

	float3 finalLightPos = UVW.x * tri[0].wPosition.xyz + UVW.y * tri[1].wPosition.xyz + UVW.z * tri[2].wPosition.xyz;//New light position

	/*Transforming the light to clip space*/
	Output.lightPosition1 = mul(lightView1, float4(finalLightPos, 1.0f));
	Output.lightPosition1 = mul(lightProjection1, Output.lightPosition1);

	Output.lightPosition2 = mul(lightView2, float4(finalLightPos, 1.0f));
	Output.lightPosition2 = mul(lightProjection2, Output.lightPosition2);

	Output.lightPosition3 = mul(lightView3, float4(finalLightPos, 1.0f));
	Output.lightPosition3 = mul(lightProjection3, Output.lightPosition3);

	Output.lightPosition4 = mul(lightView4, float4(finalLightPos, 1.0f));
	Output.lightPosition4 = mul(lightProjection4, Output.lightPosition4);

	return Output;
}
