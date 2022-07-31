
static const float PI = 3.14159265f;
static const float TO_RAD = (PI / 180);

struct SpotLight {
    float4 color;
    float3 position;
    float innerAngle;
    float3 direction;
    float outerAngle;
	float4 on;
};

struct DirectionalLight {
    float4 color;
    float3 direction;
	bool on;
};

Texture2D<float4> positions : register(t0);
Texture2D<float4> wPositions : register(t1);
Texture2D<float4> colors : register(t2);
Texture2D<float4> normals : register(t3);
Texture2D<float4> ambients : register(t4);
Texture2D<float4> speculars : register(t5);
Texture2D<float4> diffuses : register(t6);

cbuffer cb : register(b0)
{
    float Ns;//Specular expontent
    float3 kd;//Diffuse component
    float3 ks;//Specular component  
    float3 ka;//Amboient compinent
}
cbuffer lightCbDirectional : register(b1)
{
    DirectionalLight dLight;
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
    SpotLight sLights[3];
}


RWTexture2DArray<unorm float4> backbuffer : register(u6);


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
	float4 diffuse = diffuses.Load(tPos);

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
			if (!dLight.on) {
				continue;
			}
			//Diffuse
			float3 d = -normalize(dLight.direction);
			float diff = max(dot(d, normal.xyz), 0.0f);
			if (diff == 0.0f) continue;
			tempDiffuse = diff * normalize(matColor + dLight.color) * diffuse;

			//Specular
			float3 ref = -normalize(reflect(d, normal.xyz));
			float3 vec = normalize(camPos.xyz - wPosition.xyz);
			float spec = dot(ref, vec);
			if (spec >= 0.0f) {
				tempSpecular = specular * pow(abs(spec), Ns);
			}
		}
		//Spot Light
		else {
			int index = i - 1;
			if (sLights[index].on.x == 0.0f) {
				continue;
			}

			//Diffuse
			float3 lightPixelVec = normalize(sLights[index].position - wPosition.xyz);
			float3 d = -normalize(sLights[index].direction);
			float angle = acos(dot(d, lightPixelVec));
			if (angle > sLights[index].outerAngle * TO_RAD) continue;
			float lightFacotr = 1.0f;
			if (angle > sLights[index].innerAngle * TO_RAD) {
				float testAngle = angle - (sLights[index].innerAngle * TO_RAD);
				float totAngle = (sLights[index].outerAngle - sLights[index].innerAngle) * TO_RAD;
				lightFacotr = testAngle / totAngle;
				lightFacotr = 1 - lightFacotr;

			}

			float diff = max(dot(d, normal.xyz), 0.0f);
			if (diff == 0.0f) continue;
			tempDiffuse = diff * (matColor + sLights[index].color) * diffuse * lightFacotr;

			//Specular
			float3 ref = normalize(reflect(-lightPixelVec, normal.xyz));
			float3 vec = normalize(camPos.xyz - wPosition.xyz);
			float spec = max(dot(ref, vec), 0.0f);
			tempSpecular += specular * pow(spec, Ns);
			tempSpecular *= lightFacotr;

		}
		finalDiffuse += tempDiffuse;
		finalSpecular += tempSpecular;
	}
	final = finalAmbient + finalDiffuse + finalSpecular;

    backbuffer[DTid.xyz] = final;

}