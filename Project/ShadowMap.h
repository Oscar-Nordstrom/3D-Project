#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <iostream>

#include "Camera.h"
#include "Graphics.h"
#include "Structures.h"
#include "flags.h"

class ShadowMap {
public:
	ShadowMap();
	~ShadowMap();
	void Init(Graphics*& gfx, DirectionalLight* light);
	void StartFirst();
	void StartSeccond();
	void UpdateWhatShadow(int whatLight, int flag);
	void SetDirLight(DirectionalLight* dLight);
	void SetSampler(int slot = 1);
	//Needs 3 lights
	void SetSpotLights(SpotLight spotLights[]);
private:
	ID3D11DepthStencilView* GetDsView(int what);
	void DepthToSRV();
	void SetViewPort();
	bool CreateDepthStencil();
	bool LoadShaders();
	bool SetUpSampler();

	int lightTurn;

	Graphics* gfx;
	DirectionalLight* dLight;
	SpotLight* sLight1;
	SpotLight* sLight2;
	SpotLight* sLight3;

	ID3D11ShaderResourceView* shadowSRV[4];
	ID3D11DepthStencilView* dsViews[4];
	ID3D11Texture2D* dsTexture[4];
	ID3D11VertexShader* vertexShadowShader;
	ID3D11Resource* shadowRes;
	ID3D11SamplerState* samState;
	D3D11_VIEWPORT shadowViewPort;

	DirectX::XMMATRIX projection;

};