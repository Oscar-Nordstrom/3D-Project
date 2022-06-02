struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float4 wPosition : W_POSITION;
	float4 normal : NORMAL;
	float2 uv : UV;
	//positions from perespective of light
	float4 lightPosition : LIGHTPOS;//Directional Light
	float4 lightPosition2 : LIGHTPOS1;//Spot Light 1
	float4 lightPosition3 : LIGHTPOS2;//Spot Light 2
	float4 lightPosition4 : LIGHTPOS3;//Spot Light 3
};

struct PixelShaderOutput
{
	float4 position : SV_Target0;
	float4 wPosition : SV_Target1;
	float4 color : SV_Target2;
	float4 normal : SV_Target3;
	float4 ambient : SV_Target4;
	float4 specular : SV_Target5;
};

cbuffer shadowSettings : register(b0)
{
	bool ShadowsOn;
}
cbuffer lightcb : register(b1)
{
	float4 lightColor;
	float3 lightDir;
}
//Buf 2,3,4 i set


Texture2D<float4> map_Kd : register(t0); //Diffuse
Texture2D<float4> map_Ks : register(t1); //Specular
Texture2D<float4> map_Ka : register(t2); //Ambient
SamplerState samp1 : register(s0);

Texture2DArray<float4> shadowMaps : register(t3);


PixelShaderOutput main(PixelShaderInput input)
{

	bool lit = false;
	int numLights = 4;
	float shadowCoeff = 1.0f;
	for (int i = 0; i < numLights; i++) {
		if (lit) {
			break;
		}
		if (dot(normalize(lightDir), -input.normal.xyz) <= 0.0f) {
			continue;
		}

		float4 lightPositionToUse;
		//Convert to NDC
		switch (i) {
		case 0:
			lightPositionToUse = input.lightPosition;//Directional Light
			break;
		case 1:
			lightPositionToUse = input.lightPosition2;//Spot Light
			break;
		case 2:
			lightPositionToUse = input.lightPosition3;//Spot Light
			break;
		case 3:
			lightPositionToUse = input.lightPosition4;//Spot Light
			break;
		}
		lightPositionToUse.xy /= lightPositionToUse.w;
		//Translate to UV (0-1)
		float2 smTex = float2(lightPositionToUse.x * 0.5f + 0.5f, lightPositionToUse.y * (-0.5f) + 0.5f);//OK
		//Compute pixel depth for shadowing
		float depth = lightPositionToUse.z / lightPositionToUse.w;//OK


	   //Sample
		float4 sampled = shadowMaps.SampleLevel(samp1, float3(smTex, i), 0);
		//Check if shadowd
		if (sampled.r < depth)
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

	output.color = map_Kd.Sample(samp1, input.uv) * shadowCoeff;
	output.specular = map_Ks.Sample(samp1, input.uv) * shadowCoeff;
	output.ambient = map_Ka.Sample(samp1, input.uv);
	output.position = input.position;
	output.wPosition = input.wPosition;
	output.normal = input.normal;



	return output;


}