struct GSOutput
{
	float4 position : SV_POSITION;
};
struct GSInput
{
    float3 position : SV_POSITION;
};

[maxvertexcount(3)]
void main(GSInput input[3], inout TriangleStream<GSOutput> output)
{
	/*for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
		element.pos = input[i];
		output.Append(element);
	}*/
	
    //TEST
    float3 right = float3(0.0f, 0.0f, 0.0f);
    float3 up = float3(0.0f, 0.0f, 0.0f);
    matrix viewProjectionMatrix;
    //TEST
    
    const static float SIZE = 0.25f;
    //float3 right = ...;
    //float3 up = ...;
    float3 topLeft = input[0].position - right * SIZE + up * SIZE;
    float3 topRight = input[0].position + right * SIZE + up * SIZE;
    float3 bottomRight = input[0].position + right * SIZE - up * SIZE;
    GSOutput outputs[3] =
    {
        mul(viewProjectionMatrix, float4(topLeft, 1.0f))
        , mul(viewProjectionMatrix, float4(topRight, 1.0f))
        , mul(viewProjectionMatrix, float4(bottomRight, 1.0f))
    };
    output.Append(outputs[0]);
    output.Append(outputs[1]);
    output.Append(outputs[2]);
}