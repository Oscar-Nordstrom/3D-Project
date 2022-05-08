struct GSOutput
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
};
struct GSInput
{
	float4 position : SV_POSITION;
	float4x4 view : VIEW;
	float4x4 proj : PROJ;
};

cbuffer cBuf : register(b0) {
	float4 cameraPosition;
}
cbuffer cBuf2 : register(b1) {
	float4 cameraDirection;
}


[maxvertexcount(6)]
void main(point GSInput input[1], inout TriangleStream<GSOutput> output)
{

	float3 camPos = cameraPosition.xyz;
	float3 camDir = cameraDirection.xyz;

	float4x4 view_proj = mul(input[0].view, input[0].proj);
	float4x4 view = input[0].view;
	float4x4 proj = input[0].proj;

	const static float SIZE = 0.25f;

	//Billboarding start
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 right = normalize(cross(up, camDir));
	//Billboarding end

	float3 topLeft = input[0].position.xyz - (right * SIZE) + (up * SIZE);
	float3 topRight = input[0].position.xyz + (right * SIZE) +( up * SIZE);
	float3 bottomRight = input[0].position.xyz + (right * SIZE) - (up * SIZE);
	float3 bottomLeft = input[0].position.xyz -(right * SIZE) - (up * SIZE);

	/*float3 topLeft = float3(-SIZE, SIZE, 0);
	float3 topRight = float3(SIZE, SIZE, 0);
	float3 bottomRight = float3(SIZE, -SIZE, 0);
	float3 bottomLeft = float3(-SIZE, -SIZE, 0);*/

	float3 norm = normalize(cross(up, right));
	GSOutput toOutput;
	

	toOutput.position = mul(view, float4(topLeft, 1.0f));
	toOutput.position = mul(proj, toOutput.position);
	//toOutput.position.w = 1.0f;
	toOutput.uv = float2(0.0f, 0.0f);
	toOutput.normal = norm;
	output.Append(toOutput);


	toOutput.position = mul(view, float4(topRight, 1.0f));
	toOutput.position = mul(proj, toOutput.position);
	//toOutput.position.w = 1.0f;
	toOutput.uv = float2(1.0f, 0.0f);
	toOutput.normal = norm;
	output.Append(toOutput);


	toOutput.position = mul(view, float4(bottomRight, 1.0f));
	toOutput.position = mul(proj, toOutput.position);
	//toOutput.position.w = 1.0f;
	toOutput.uv = float2(1.0f, 1.0f);
	toOutput.normal = norm;
	output.Append(toOutput);

	output.RestartStrip();
	toOutput.normal = normalize(cross(up, right));


	toOutput.position = mul(view, float4(topLeft, 1.0f));
	toOutput.position = mul(proj, toOutput.position);
	//toOutput.position.w = 1.0f;
	toOutput.uv = float2(0.0f, 0.0f);
	toOutput.normal = norm;
	output.Append(toOutput);


	toOutput.position = mul(view, float4(bottomRight, 1.0f));
	toOutput.position = mul(proj, toOutput.position);
	//toOutput.position.w = 1.0f;
	toOutput.uv = float2(1.0f, 1.0f);
	toOutput.normal = norm;
	output.Append(toOutput);


	toOutput.position = mul(view, float4(bottomLeft, 1.0f));
	toOutput.position = mul(proj, toOutput.position);
	//toOutput.position.w = 1.0f;
	toOutput.uv = float2(0.0f, 1.0f);
	toOutput.normal = norm;
	output.Append(toOutput);
}