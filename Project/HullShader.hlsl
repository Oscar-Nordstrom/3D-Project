cbuffer camcb : register(b0)
{
    float4 camPos;
}

cbuffer cBuf : register(b1)
{
    bool tess;
};

struct VS_CONTROL_POINT_OUTPUT
{
    float4 position : SV_POSITION;
    float4 wPosition : W_POSITION;
    float4 normal : NORMAL;
    float2 uv : UV;
   // float4 lightPosition : LIGHTPOS;
};

struct HS_CONTROL_POINT_OUTPUT
{
    float4 position : SV_POSITION;
    float4 wPosition : W_POSITION;
    float4 normal : NORMAL;
    float2 uv : UV;
   // float4 lightPosition : LIGHTPOS;
};

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor;
	float InsideTessFactor			: SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 3

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip, uint PatchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT Output;
    
    //Distance between the camera and the current points
    float one = pow(0.5, pow(2, camPos.x - ip[0].position.x) + pow(2, camPos.y - ip[0].position.y) + pow(2, camPos.z - ip[0].position.z));
    float two = pow(0.5, pow(2, camPos.x - ip[1].position.x) + pow(2, camPos.y - ip[1].position.y) + pow(2, camPos.z - ip[1].position.z));
    float three = pow(0.5, pow(2, camPos.x - ip[2].position.x) + pow(2, camPos.y - ip[2].position.y) + pow(2, camPos.z - ip[2].position.z));
    
    float distance = (one + two + three) / 3;
    if (distance < 1)
    {
        distance = 1;
    }
    float gTessFactor = 15 / distance;
    if (!tess) {
        gTessFactor = 1;
    }

    Output.EdgeTessFactor[0] = gTessFactor;
    Output.EdgeTessFactor[1] = gTessFactor;
    Output.EdgeTessFactor[2] = gTessFactor;

    Output.InsideTessFactor = gTessFactor;

	return Output;

}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]

HS_CONTROL_POINT_OUTPUT main( InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip, uint i : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID )
{
	HS_CONTROL_POINT_OUTPUT Output;

	//Output.lightPosition = ip[i].lightPosition;
    Output.wPosition = ip[i].wPosition;
	Output.position = ip[i].position;
	Output.normal = ip[i].normal;
    Output.uv = ip[i].uv;

	return Output;
}
