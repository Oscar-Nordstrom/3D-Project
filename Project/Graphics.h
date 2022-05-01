#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>
#include "Structures.h"
#include "flags.h"

#include "imgui.h"
#include"imgui_impl_win32.h"
#include"imgui_impl_dx11.h"


class Graphics {
public:
	Graphics(int width, int height, HWND& window);
	~Graphics();
	ID3D11Device*& GetDevice();
	ID3D11DeviceContext*& GetContext();

	void StartFrame(float r, float g, float b, int flag = NORMAL);
	void EndFrame(int width, int height, int flag = NORMAL);

	void SetProjection(DirectX::XMMATRIX proj);
	void SetCamera(DirectX::XMMATRIX cam);
	DirectX::XMMATRIX GetProjection()const;
	DirectX::XMMATRIX GetCamera()const;
	const TextureRT* GetGBuffer()const;
	ID3D11Texture2D* GetBackBuffer()const;
	int GetWidth();
	int GetHeight();
	void EnableImGui();
	void DisableImGui();
	bool ImGuiEnabled()const;

	void present();

	void SetNormalViewPort();


private:

	float timeI = 0;

	int w, h;

	void SetUpImGui(HWND& window);
	void ImGuiStart();
	void ImGuiEnd();
	bool CreateDeviceAndSwapchain(int width, int height, HWND& window);
	bool CreateDepthStencilView(int width, int height);
	void SetViewport(int width, int height);
	bool SetUpSampler(ID3D11Device* device, ID3D11SamplerState*& samState);
	bool SetUpGbuffer(ID3D11Device* device, int width, int height);
	bool CreateUAV(ID3D11Device* device, int width, int height);
	bool CreateRTV(ID3D11Device* device, int width, int height);

	DirectX::XMMATRIX projection;
	DirectX::XMMATRIX camera;

	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	IDXGISwapChain* swapChain;
	ID3D11Texture2D* dsTexture;
	ID3D11DepthStencilView* dsView;
	ID3D11UnorderedAccessView* uav;
	ID3D11SamplerState* samState;
	ID3D11UnorderedAccessView* nullUav = nullptr;
	ID3D11RenderTargetView* nullRtv[6] = { nullptr, nullptr, nullptr, nullptr, nullptr };
	ID3D11ShaderResourceView* nullSrv[6] = { nullptr, nullptr, nullptr, nullptr, nullptr };
	TextureRT gBuffer[numGbufs];
	ID3D11RenderTargetView* renderTargets[6];
	ID3D11RenderTargetView* rtv;
	ID3D11ShaderResourceView* shaderResources[6];
	D3D11_VIEWPORT viewport;

	bool imGuiEnabled;


};