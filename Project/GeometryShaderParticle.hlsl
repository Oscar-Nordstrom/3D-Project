struct GSOutput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
};
struct GSInput
{
    float4 position : SV_POSITION;
    float4x4 world : WORLD;
    float4x4 view : VIEW;
    float4x4 proj : PROJ;
};

cbuffer cBuf : register(b0) { 
    float4 cameraPosition;
}

[maxvertexcount(6)]
void main(point GSInput input[1], inout TriangleStream<GSOutput> output)
{
	

    matrix viewProjectionMatrix = input[0].view * input[0].proj;
    float3 camPos = cameraPosition.xyz;

    
    float4x4 worldView = mul(input[0].world, input[0].view);

    const static float SIZE = 100.0;

    //Billboarding start
    float3 camObjVec = (input[0].position.xyz - camPos);
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 right = float3(1.0f, 0.0f, 0.0f);//cross(up.xyz, camObjVec.xyz);
    //Billboarding end

    float3 topLeft = input[0].position.xyz - right * SIZE + up * SIZE;
    float3 topRight = input[0].position.xyz + right * SIZE + up * SIZE;
    float3 bottomRight = input[0].position.xyz + right * SIZE - up * SIZE;
    float3 bottomLeft = input[0].position.xyz - right * SIZE - up * SIZE;



    GSOutput toOutput;
    toOutput.position = mul(viewProjectionMatrix, float4(topLeft, 1.0f));
    toOutput.normal = normalize(float3(worldView[0][2], worldView[1][2], worldView[2][2]));
    toOutput.uv = float2(0.0f, 0.0f);
    output.Append(toOutput);

    toOutput.position = mul(viewProjectionMatrix, float4(topRight, 1.0f));
    toOutput.uv = float2(1.0f, 0.0f);
    output.Append(toOutput);

    toOutput.position = mul(viewProjectionMatrix, float4(bottomRight, 1.0f));
    toOutput.uv = float2(1.0f, 1.0f);
    output.Append(toOutput);

    toOutput.position = mul(viewProjectionMatrix, float4(topLeft, 1.0f));
    toOutput.uv = float2(0.0f, 0.0f);
    output.Append(toOutput);

    toOutput.position = mul(viewProjectionMatrix, float4(bottomRight, 1.0f));
    toOutput.uv = float2(1.0f, 1.0f);
    output.Append(toOutput);

    toOutput.position = mul(viewProjectionMatrix, float4(bottomLeft, 1.0f));
    toOutput.uv = float2(0.0f, 1.0f);
    output.Append(toOutput);
}