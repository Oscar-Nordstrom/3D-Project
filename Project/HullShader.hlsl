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
	float InsideTessFactor			: SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 3

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip, uint PatchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT Output;
    
    //Distance between the camera and the current points
   /* float one = pow(0.5, pow(2, camPos.x - ip[0].position.x) + pow(2, camPos.y - ip[0].position.y) + pow(2, camPos.z - ip[0].position.z));
    float two = pow(0.5, pow(2, camPos.x - ip[1].position.x) + pow(2, camPos.y - ip[1].position.y) + pow(2, camPos.z - ip[1].position.z));
    float three = pow(0.5, pow(2, camPos.x - ip[2].position.x) + pow(2, camPos.y - ip[2].position.y) + pow(2, camPos.z - ip[2].position.z));
    */
    float one = abs(camPos.x - ip[0].position.x);
    float two = abs(camPos.x - ip[1].position.x);
    float three = abs(camPos.x - ip[2].position.x);
    float distance = (one + two + three) / 3;
    if (distance < 1)
    {
        distance = 1;
    }
    float gTessFactor = 15 / distance;
    if (!tess || (gTessFactor < 1.0f)) {
        gTessFactor = 1.0f;
    }

    Output.EdgeTessFactor[0] = gTessFactor;
    Output.EdgeTessFactor[1] = gTessFactor;
    Output.EdgeTessFactor[2] = gTessFactor;

    Output.InsideTessFactor = gTessFactor;

	return Output;

}

[domain("tri")]//We have a triangle
[outputtopology("triangle_cw")]//Handle triangle clockwise
[partitioning("integer")]//Rounds floats to ints in range 1-64
[outputcontrolpoints(3)]//Number of control points
[patchconstantfunc("CalcHSPatchConstants")]//Name of function above

HS_CONTROL_POINT_OUTPUT main( InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip, uint i : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID )
{
	HS_CONTROL_POINT_OUTPUT Output;

    Output.wPosition = ip[i].wPosition;
	Output.position = ip[i].position;
	Output.normal = ip[i].normal;
    Output.uv = ip[i].uv;

	return Output;
}