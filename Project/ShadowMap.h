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
	ShadowMap(Graphics*& gfx, DirectionalLight* light);
	~ShadowMap();
	void StartFirst(DirectX::XMFLOAT3 pos, int flag = SPOT_LIGHT);
	void EndFirst();
	void SetDirLight(DirectionalLight* dLight);
	void SetSpotLight(SpotLight* sLight);
private:
	bool CreateDepthStencil();
	bool LoadShaders();

	Graphics*& gfx;
	Camera cam;
	ID3D11VertexShader* vertexShadowShader;
	ID3D11Texture2D* dsTexture;
	ID3D11DepthStencilView* dsView;
	DirectionalLight* dLight;
	SpotLight* sLight;
};