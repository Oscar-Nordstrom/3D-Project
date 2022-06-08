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

struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float4 wPosition : W_POSITION;
	float4 normal : NORMAL;
	float2 uv : UV;
	//positions from perespective of light
	float4 lightPosition1 : LIGHTPOS1;//Directional Light
	float4 lightPosition2 : LIGHTPOS2;//Spot Light 1
	float4 lightPosition3 : LIGHTPOS3;//Spot Light 2
	float4 lightPosition4 : LIGHTPOS4;//Spot Light 3
};

struct PixelShaderOutput
{
	float4 position : SV_Target0;
	float4 wPosition : SV_Target1;
	float4 color : SV_Target2;
	float4 normal : SV_Target3;
	float4 ambient : SV_Target4;
	float4 specular : SV_Target5;
	float4 diffuse : SV_Target6;
};

cbuffer shadowSettings : register(b0)
{
	bool ShadowsOn;
}
cbuffer lightcb : register(b1)
{
	DirectionalLight dLight;
}
cbuffer lightCbSpot : register(b2)
{
	SpotLight sLights[3];
}


Texture2D<float4> map_Kd : register(t0); //Diffuse
Texture2D<float4> map_Ks : register(t1); //Specular
Texture2D<float4> map_Ka : register(t2); //Ambient
SamplerState samp1 : register(s0);
SamplerState shadowSamp : register(s1);

Texture2D<float4> shadowMap1 : register(t3);//Directional Light
Texture2D<float4> shadowMap2 : register(t4);//SpotLight 1
Texture2D<float4> shadowMap3 : register(t5);//SpotLight 2
Texture2D<float4> shadowMap4 : register(t6);//SpotLight 3


PixelShaderOutput main(PixelShaderInput input)
{
	const float bias = 0.0001f;
	float4 zero = float4(0.0f, 0.0f, 0.0f, 0.0f);
	bool lit = false;
	int numLights = 4;
	float shadowCoeff = 1.0f;
	for (int i = 0; i < numLights; i++) {
		bool lightIsOn = true;
		float4 lightPositionToUse = zero;
		switch (i) {
		case 0:
			lightPositionToUse = input.lightPosition1;//Directional Light
			if (!dLight.on) {
				lightIsOn = false;
			}
			break;
		case 1:
			lightPositionToUse = input.lightPosition2;//Spot Light
			if (sLights[0].on.x == 0.0f) {
				lightIsOn = false;
			}
			break;
		case 2:
			lightPositionToUse = input.lightPosition3;//Spot Light
			if (sLights[1].on.x == 0.0f) {
				lightIsOn = false;
			}
			break;
		case 3:
			lightPositionToUse = input.lightPosition4;//Spot Light
			if (sLights[2].on.x == 0.0f) {
				lightIsOn = false;
			}
			break;
		}
		if (!lightIsOn) {
			continue;
		}
		//Convert to NDC
		lightPositionToUse.xy /= lightPositionToUse.w;
		//Translate to UV (0-1)
		float2 smTex = float2(lightPositionToUse.x * 0.5f + 0.5f, lightPositionToUse.y * (-0.5f) + 0.5f);//OK
		//Compute pixel depth for shadowing
		float depth = lightPositionToUse.z / lightPositionToUse.w;//OK
		//Sample
		float4 sampled = zero;
		switch (i) {
		case 0:
			sampled = shadowMap1.Sample(shadowSamp, smTex);
			break;
		case 1:
			sampled = shadowMap2.Sample(shadowSamp, smTex);
			break;
		case 2:
			sampled = shadowMap3.Sample(shadowSamp, smTex);
			break;
		case 3:
			sampled = shadowMap4.Sample(shadowSamp, smTex);
			break;
		}
		

		//Check if shadowd
		if (sampled.r+bias < depth)
		{
			//If the light sees is less than what we see, there is a shadow
			shadowCoeff = 0.0f;
		}
		else {
			lit = true;
		}
	}
	if (!ShadowsOn) {
		shadowCoeff = 1.0f;
	}

	PixelShaderOutput output;

	output.color = map_Kd.Sample(samp1, input.uv);
	output.specular = map_Ks.Sample(samp1, input.uv) * shadowCoeff;
	output.diffuse = map_Kd.Sample(samp1, input.uv) * shadowCoeff;
	output.ambient = map_Ka.Sample(samp1, input.uv);
	output.position = input.position;
	output.wPosition = input.wPosition;
	output.normal = input.normal;



	return output;


}