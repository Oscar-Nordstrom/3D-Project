cbuffer camcb : register(b0)
{
    float4 camPos;
}

cbuffer cBuf : register(b1)
{
    bool tess;
};

cbuffer cBuf : register(b2)
{
    float3 meshPos;
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
    
    /*
    Take average distance to all three control points, and check that how much we need to tesselate.
    */
    float one = pow(pow(camPos.x - ip[0].position.x, 2) + pow(camPos.y - ip[0].position.y, 2) + pow(camPos.z - ip[0].position.z, 2), 0.5f);
    float two = pow(pow(camPos.x - ip[1].position.x, 2) + pow(camPos.y - ip[1].position.y, 2) + pow(camPos.z - ip[1].position.z, 2), 0.5f);
    float three = pow(pow(camPos.x - ip[2].position.x, 2) + pow(camPos.y - ip[2].position.y, 2) + pow(camPos.z - ip[2].position.z, 2), 0.5f);
    float distance = (one + two + three) / 3;
    if (distance < 1)
    {
        distance = 1;
    }
    float inTessFactor = 10 / distance;
    if (!tess || (inTessFactor < 1.0f)) {
        inTessFactor = 1.0f;
    }

    Output.InsideTessFactor = inTessFactor;

    float dist = pow(pow(camPos.x - meshPos.x, 2) + pow(camPos.y - meshPos.y, 2) + pow(camPos.z - meshPos.z, 2), 0.5f);
    float outTessFactor = 15 / dist;
    //Outer factor is shared between all corners
    Output.EdgeTessFactor[0] = outTessFactor;
    Output.EdgeTessFactor[1] = outTessFactor;
    Output.EdgeTessFactor[2] = outTessFactor;


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