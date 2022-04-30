struct GSOutput
{
	float4 position :  SV_POSITION;
};
struct GSInput
{
    float4 position : SV_POSITION;
};

[maxvertexcount(6)]
void main(point GSInput input[1], inout TriangleStream<GSOutput> output)
{
	
    matrix viewProjectionMatrix = {1.0f, 0.0f, 0.0f, 0.0f,
                                   0.0f, 1.0f, 0.0f, 0.0f, 
                                   0.0f, 0.0f, 1.0f, 0.0f,
                                   0.0f, 0.0f, 0.0f, 1.0f };//This needs to be changed to the real matrix
    float3 camPos = float3(0.0f, 0.0f, 0.0f);//This needs to be the real cam pos later

    
    const static float SIZE = 0.25f;

    //Billboarding start
    float3 camObjVec = (input[0].position.xyz - camPos);
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 right = cross(up.xyz, camObjVec.xyz);
    //Billboarding end

    float3 topLeft = input[0].position.xyz - right * SIZE + up * SIZE;  //We work in wordl sapce here
    float3 topRight = input[0].position.xyz + right * SIZE + up * SIZE;
    float3 bottomRight = input[0].position.xyz + right * SIZE - up * SIZE;
    float3 bottomLeft = input[0].position.xyz - right * SIZE - up * SIZE;

    GSOutput outputs[6] =
    {
        mul(viewProjectionMatrix, float4(topLeft, 1.0f)),
        mul(viewProjectionMatrix, float4(topRight, 1.0f)),
        mul(viewProjectionMatrix, float4(bottomRight, 1.0f)),
        mul(viewProjectionMatrix, float4(topLeft, 1.0f)),
        mul(viewProjectionMatrix, float4(bottomRight, 1.0f)),
        mul(viewProjectionMatrix, float4(bottomLeft, 1.0f))
    };
    output.Append(outputs[0]);
    output.Append(outputs[1]);
    output.Append(outputs[2]);
    output.Append(outputs[3]);
    output.Append(outputs[4]);
    output.Append(outputs[5]);
}