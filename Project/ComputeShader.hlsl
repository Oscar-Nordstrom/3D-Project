
Texture2D<float4> positions : register(t0);
Texture2D<float4> wPositions : register(t1);
Texture2D<float4> colors : register(t2);
Texture2D<float4> normals : register(t3);
Texture2D<float4> ambients : register(t4);
Texture2D<float4> speculars : register(t5);

cbuffer cb : register(b0)
{
	float s;//Specular expontent
	float3 kd;//Diffuse component
	float3 ks;//Specular component  
	float3 ka;//Amboient compinent
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
cbuffer cBuf2 : register(b3) {
	float4 cameraDirection;
}
cbuffer lightcb : register(b4)
{
	float4 colorSpot[3];
	float3 posSpot[3];
	float3 dirSpot[3];
	float inner;
	float outer;
}

RWBuffer<float> particles;

RWTexture2D<unorm float4> backbuffer : register(u0);

[numthreads(20, 20, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	float3 neg = float3(-1.0f, -1.0f, -1.0f);
	float4 final = float4(0.0f, 0.0f, 0.0f, 0.0f);;
	int2 texPos = int2(0, 0);
	texPos.x = DTid.x;
	texPos.y = DTid.y;
	int3 tPos = int3(texPos, 0);

	float4 matColor = colors.Load(tPos);
	// float4 position = positions.Load(tPos);
   //  float4 wPosition = wPositions.Load(tPos);
	float4 normal = normalize(normals.Load(tPos));
	float4 ambient;
	if (ka.x == neg.x || ka.y == neg.y || ka.z == neg.z) {
		ambient = ambients.Load(tPos);
	}
	else {
		ambient = float4(ka.xyz, 0.0f);
	}
	 //float4 specular = speculars.Load(tPos);


	float4 finalAmbient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 finalDiffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	// float4 finalSpecular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	 //Ambient calculations
	finalAmbient = ambient * matColor;

	//Diffuse
	float3 norm = normal.xyz;
	float3 lightDir = normalize(dirSpot[0]);
	float diff = max(dot(norm, lightDir), 0.0f);
	finalDiffuse = diff * matColor;

	final = finalAmbient + finalDiffuse;

	//Ambient calculations
	//finalAmbient = ambient * matColor;
	//float3 d;
	//for (int i = 0; i < 4; i++) {
	//    if (i == 0) {
	//       d = normalize(dir);
	//    }
	//    else {
	//        d = normalize(dirSpot[i - 1]);
	//    }
	//    normal = -normalize(normal);
	//    //Diffuse calculateions
	//    float diff = dot(normal.xyz, d);
	//    if (diff >= 0.0f)
	//    {
	//        finalDiffuse += diff * (matColor + color);// *float4(kd.xyz, 0.0f);
	//    }
	//    //Specular calculations
	//    if (diff >= 0.0f)
	//    {
	//        float3 r = normalize(reflect(d, normal.xyz));
	//        float3 v = normalize(cameraDirection.xyz);//normalize(camPos.xyz - wPosition.xyz);
	//        float spec = dot(r, v);
	//        if (spec >= 0.0f)
	//        {
	//            finalSpecular += specular * pow(spec, s);
	//        }

	//    }

	//}
	//final = finalAmbient + finalDiffuse + finalSpecular;

	backbuffer[DTid.xy] = final;

}