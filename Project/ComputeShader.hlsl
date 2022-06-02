
Texture2D<float4> positions : register(t0);
Texture2D<float4> wPositions : register(t1);
Texture2D<float4> colors : register(t2);
Texture2D<float4> normals : register(t3);
Texture2D<float4> ambients : register(t4);
Texture2D<float4> speculars : register(t5);

cbuffer cb : register(b0)
{
	float Ns;//Specular expontent
	float3 kd;//Diffuse component
	float3 ks;//Specular component  
	float3 ka;//Amboient compinent
}
cbuffer lightCbDirectional : register(b1)
{
	float4 colorDirLight;
	float3 directionDirLight;
}
cbuffer camcb : register(b2)
{
	float4 camPos;
}
cbuffer cBuf2 : register(b3) {
	float4 cameraDirection;
}
cbuffer lightCbSpot : register(b4)
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
	float4 zero = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 final = float4(0.0f, 0.0f, 0.0f, 0.0f);;
	int2 texPos = int2(0, 0);
	texPos.x = DTid.x;
	texPos.y = DTid.y;
	int3 tPos = int3(texPos, 0);

	//Load all values
	float4 position = positions.Load(tPos);
	float4 wPosition = wPositions.Load(tPos);
	float4 matColor = colors.Load(tPos);
	float4 normal = normalize(normals.Load(tPos));
	float4 ambient = ambients.Load(tPos);
	float4 specular = speculars.Load(tPos);

	//Set all light to zero
	float4 finalAmbient = zero;
	float4 finalDiffuse = zero;
	float4 finalSpecular = zero;



	//Ambient calculations
	finalAmbient = ambient * matColor;


	for (int i = 0; i < 4; i++) {
		if (matColor.x == zero.x && matColor.y == zero.y && matColor.z == zero.z) {
			continue;
		}
		float4 tempDiffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 tempSpecular = float4(0.0f, 0.0f, 0.0f, 0.0f);
		//Directionl light
		if (i == 0) {
			//Diffuse
			float3 d = -normalize(directionDirLight);
			float diff = max(dot(d, normal.xyz), 0.0f);
			if (diff == 0.0f) continue;
			tempDiffuse = diff * normalize(matColor + colorDirLight);

			//Specular
			float3 ref = -normalize(reflect(d, normal.xyz));
			float3 vec = normalize(camPos.xyz - wPosition.xyz);
			float spec = dot(ref, vec);
			if (spec >= 0.0f) {
				tempSpecular = specular * pow(spec, Ns);
			}
		}
		//Spot Light
		else {
			int index = i - 1;
			//float3 lightToPixelVec = posSpot[index] - wPosition.xyz;
			//float coneCalc = pow(max(dot(-lightToPixelVec, dirSpot[index]), 0.0f), outer);
			//if (coneCalc <= 0.0f)continue;

			//Diffuse
			float3 d = -normalize(dirSpot[index]);
			float diff = max(dot(d, normal.xyz), 0.0f);
			if (diff == 0.0f) continue;
			//tempDiffuse = diff * (matColor + colorSpot[i]);

			//Specular
			float3 ref = normalize(reflect(d, normal.xyz));
			float3 vec = normalize(camPos.xyz - wPosition.xyz);
			float spec = max(dot(ref, vec), 0.0f);
			//tempSpecular += specular * pow(spec, Ns);
		}
		finalDiffuse += tempDiffuse;
		finalSpecular += tempSpecular;
	}
	final = finalAmbient + finalDiffuse + finalSpecular;

	backbuffer[DTid.xy] = final;
}