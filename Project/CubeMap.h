#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Graphics.h"
#include "Camera.h"
#include "flags.h"
#include <cassert>



class CubeMap {
public:
	CubeMap(Graphics*& gfx);
	~CubeMap();
	void Set(ID3D11DeviceContext* context, int num);
	void SetSeccond(Graphics*& gfx);
	void SetEnd(Graphics*& gfx);
	void Clear(ID3D11DeviceContext* context);
	Camera& GetCam();
	DirectX::XMMATRIX GetProj();
private:
	bool setUpTextures(ID3D11Device*& device);
	bool SetUpSrvs(ID3D11Device*& device);
	bool SetUpRtvs(ID3D11Device*& device);
	bool SetUpUavs(ID3D11Device*& device);
	bool CreateDepthStencilView(ID3D11Device*& device);
	bool LoadShader(Graphics*& gfx);
	void SetViewport();
private:
	ID3D11Texture2D* tex;
	ID3D11ShaderResourceView* srv;
	ID3D11ShaderResourceView* nullSrv;
	ID3D11UnorderedAccessView* nullUav;
	ID3D11RenderTargetView* rtv[6];
	ID3D11UnorderedAccessView* uav[6];
	ID3D11Texture2D* dsTex;
	ID3D11DepthStencilView* dsView;
	ID3D11PixelShader* pShader;
	ID3D11ComputeShader* cShader;

	Camera cam;
	DirectX::XMMATRIX proj;
	D3D11_VIEWPORT viewPort;

	
	DirectX::XMFLOAT3 pos;

};