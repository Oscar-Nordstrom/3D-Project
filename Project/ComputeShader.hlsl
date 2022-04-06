
Texture2D<float4> positions : register(t0);
Texture2D<float4> wPositions : register(t1);
Texture2D<float4> colors : register(t2);
Texture2D<float4> normals : register(t3);
Texture2D<float4> ambients : register(t4);
Texture2D<float4> speculars : register(t5);

cbuffer cb : register(b0)
{
    float s;//Specular expontent
}
cbuffer lightcb : register(b1)
{
    float4 color;
    float3 dir;
}
cbuffer camcb : register(b2)
{
    float4 camPos;
}

RWTexture2D<unorm float4> backbuffer : register(u0);



[numthreads(20, 20, 1)]

void main(uint3 DTid : SV_DispatchThreadID)
{
    
    float4 final = float4(0.0f, 0.0f, 0.0f, 0.0f);;
    int2 texPos = int2(0, 0);
    texPos.x = DTid.x;
    texPos.y = DTid.y;
    int3 tPos = int3(texPos, 0);
    
    float4 matColor = colors.Load(tPos);
    float4 position = positions.Load(tPos);
    float4 wPosition = wPositions.Load(tPos);
    float4 normal = normalize(normals.Load(tPos));
    float4 ambient = ambients.Load(tPos);
    float4 specular = speculars.Load(tPos);
    
    float4 finalAmbient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 finalDiffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 finalSpecular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    //Do light calculations
    
    //Ambient calculations
    finalAmbient = ambient * matColor;
    
    //Diffuse calculateions
    float diff = dot(-dir, normal.xyz);
    if (diff >= 0.0f)
    {
        finalDiffuse = diff * (matColor + color);

    }

    //Specular calculations
    if (diff >= 0.0f)
    {
        float3 r = -normalize(reflect(-dir, normal.xyz));
        float3 v = normalize(camPos.xyz - wPosition.xyz);
        float spec = dot(r, v);
        if (spec >= 0.0f)
        {
            finalSpecular = specular * pow(spec, s);
        }

    }
    
    final = finalAmbient + finalDiffuse + finalSpecular;
    
    backbuffer[DTid.xy] = final;

}