#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <iostream>

#include "Camera.h"
#include "Graphics.h"
#include "Structures.h"

class ShadowMap {
public:
	ShadowMap(Graphics* gfx, DirectionalLight* light);
	~ShadowMap();
	void SetCamPos(DirectX::XMFLOAT3 pos);
	void SetCamDir(DirectX::XMFLOAT3 dir);
	void SetShadowMap();
	void BindDepthResource();
private:
	bool CreateDepthStencil();
	bool LoadShaders();
	bool CreateSRV();
	bool CreateConstantBuffer();
	bool UpdateConstantBuffer();

	Camera cam;
	DirectX::XMMATRIX projection;
	DirectionalLight* light;
	Graphics* gfx;
	ID3D11VertexShader* vertexShader;
	ID3D11Texture2D* dsTexture;
	ID3D11DepthStencilView* dsView;
	ID3D11ShaderResourceView* srv;
	ID3D11Buffer* constantBuffer;

};