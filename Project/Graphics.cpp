#include "Graphics.h"


Graphics::Graphics(int width, int height, HWND& window)
{

#if _DEBUG
	shaderDir = "../Debug";
#else 
	shaderDir = "../Release";
#endif

	w = width;
	h = height;


	assert(CreateDeviceAndSwapchain(width, height, window)&& "Failed to create device and swapchain.");
	assert(SetUpGbuffer(device, width, height) && "Failed to set up G buffer.");
	assert(SetUpGbufferCubeMap(device, width, height)&& "Failed to set up G buffer cubemap.");
	assert(CreateDepthStencilView(width, height)&& "Failed to create depth stencil view.");
	assert(CreateUAV(device, width, height)&& "Failed to create uav.");
	assert(CreateRTV(device, width, height)&& "Failed to create render target view.");



	for (int i = 0; i < numGbufs; i++) {
		renderTargets[i] = gBuffer[i].rtv;
		shaderResources[i] = gBuffer[i].srv;

		renderTargetsCubeMap[i] = gBufferCubeMap[i].rtv;
		shaderResourcesCubeMap[i] = gBufferCubeMap[i].srv;
	}

	deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(3, renderTargets, dsView, 3, 1, &uav, nullptr);

	assert(SetUpSampler(device, samState)&& "Failed to set up sampler.");
	SetViewport(width, height);
	deviceContext->RSSetViewports(1, &viewport);
	deviceContext->PSSetSamplers(0, 1, &samState);

	SetUpImGui(window);
}

Graphics::~Graphics()
{
	if (device)device->Release();
	if (deviceContext)deviceContext->Release();
	if (swapChain)swapChain->Release();
	if (dsTexture)dsTexture->Release();
	if (dsTextureCubeMap)dsTextureCubeMap->Release();
	if (dsView)dsView->Release();
	if (dsViewCubeMap)dsViewCubeMap->Release();
	if (samState)samState->Release();
	if (uav)uav->Release();
	if (rtv)rtv->Release();

	for (int i = 0; i < numGbufs; i++) {
		if (gBuffer[i].texture)gBuffer[i].texture->Release();
		if (gBuffer[i].rtv)gBuffer[i].rtv->Release();
		if (gBuffer[i].srv)gBuffer[i].srv->Release();

		if (gBufferCubeMap[i].texture)gBufferCubeMap[i].texture->Release();
		if (gBufferCubeMap[i].rtv)gBufferCubeMap[i].rtv->Release();
		if (gBufferCubeMap[i].srv)gBufferCubeMap[i].srv->Release();

	}

	ImGui_ImplWin32_Shutdown();
	ImGui_ImplDX11_Shutdown();
	ImGui::DestroyContext();
}

ID3D11Device*& Graphics::GetDevice()
{
	return device;
}

ID3D11DeviceContext*& Graphics::GetContext()
{
	return deviceContext;
}

void Graphics::StartFrame(float r, float g, float b, int flag)
{
	if (flag == NORMAL) {
		timeI += 0.005f;
		if (timeI >= 1.0f) {
			timeI = 0;
		}
		ImGuiStart();
		deviceContext->RSSetViewports(1, &viewport);
		deviceContext->CSSetShaderResources(0, numGbufs, nullSrv);
		deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(numGbufs, renderTargets, dsView, numGbufs, 1, &uav, nullptr);
		const float color[] = { r, g, b, 1.0f };
		deviceContext->ClearUnorderedAccessViewFloat(uav, color);
		for (int i = 0; i < numGbufs; i++) {
			deviceContext->ClearRenderTargetView(renderTargets[i], color);
		}
		deviceContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
		
	}
	else if (flag == CUBE_MAP) {
		deviceContext->CSSetShaderResources(0, numGbufs, nullSrv);
		deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(numGbufs, renderTargetsCubeMap, dsViewCubeMap, numGbufs, 1, &uav, nullptr);
		const float color[] = { r, g, b, 1.0f };
		deviceContext->ClearUnorderedAccessViewFloat(uav, color);
		for (int i = 0; i < numGbufs; i++) {
			deviceContext->ClearRenderTargetView(renderTargetsCubeMap[i], color);
		}
		deviceContext->ClearDepthStencilView(dsViewCubeMap, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	}
	else if (flag == CUBE_MAP_TWO) {
		//deviceContext->CSSetShaderResources(0, numGbufs, nullSrv);
		//deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(numGbufs, renderTargets, dsView, numGbufs, 1, &uav, nullptr);
		deviceContext->OMSetRenderTargets(1, &rtv, dsViewCubeMap);
		const float color[] = { r, g, b, 1.0f };
		/*deviceContext->ClearUnorderedAccessViewFloat(uav, color);
		for (int i = 0; i < numGbufs; i++) {
			deviceContext->ClearRenderTargetView(renderTargets[i], color);
		}*/
		deviceContext->ClearDepthStencilView(dsViewCubeMap, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
		deviceContext->CSSetUnorderedAccessViews(6, 1, &nullUav, 0);
	}
	else if (flag == SHADOW) {
		deviceContext->RSSetViewports(1, &viewport);
	}
	else if (flag == PARTICLE) {
		deviceContext->RSSetViewports(1, &viewport);
		deviceContext->CSSetShaderResources(0, numGbufs, nullSrv);
		deviceContext->OMSetRenderTargets(1, &renderTargets[2], dsView);
		const float color[] = { r, g, b, 1.0f };
		for (int i = 0; i < numGbufs; i++) {
			deviceContext->ClearRenderTargetView(renderTargets[i], color);
		}
		deviceContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	}
}

void Graphics::EndFrame(int width, int height, int flag)
{
	if (flag == NORMAL) {
		deviceContext->OMSetRenderTargets(numGbufs, nullRtv, dsView);
		deviceContext->CSSetShaderResources(0, numGbufs, shaderResources);
		deviceContext->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
		deviceContext->Dispatch(width / 20, height / 20, 1);
		deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUav, nullptr);
		deviceContext->CSSetUnorderedAccessViews(1, 1, &nullUav, nullptr);
		deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, renderTargets, dsView, 0, 1, &uav, nullptr);
		ImGuiEnd();
		swapChain->Present(1, 0);
	}
	else if (flag == CUBE_MAP) {
		deviceContext->OMSetRenderTargets(numGbufs, nullRtv, dsView);
		deviceContext->CSSetShaderResources(0, numGbufs, shaderResourcesCubeMap);
		deviceContext->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
		deviceContext->Dispatch(width / 20, height / 20, 1);
		deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUav, nullptr);
		deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, renderTargetsCubeMap, dsView, 0, 1, &uav, nullptr);
		//swapChain->Present(1, 0);
	}
	else if (flag == CUBE_MAP_TWO) {
		deviceContext->OMSetRenderTargets(numGbufs, nullRtv, dsView);
		//deviceContext->CSSetShaderResources(0, numGbufs, shaderResources);
		//deviceContext->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
		//deviceContext->Dispatch(width / 20, height / 20, 1);
		//deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUav, nullptr);
		deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, renderTargetsCubeMap, dsView, 0, 1, &uav, nullptr);
		//deviceContext->OMSetRenderTargets(1, &rtv, dsView);
	}
	else if (flag == PARTICLE) {
		deviceContext->OMSetRenderTargets(numGbufs, nullRtv, dsView);
		deviceContext->CSSetShaderResources(0, 1, &shaderResources[2]);
		deviceContext->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
		deviceContext->Dispatch(NUM_PARTICLES/2, 1, 1);
		deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUav, nullptr);
		deviceContext->CSSetUnorderedAccessViews(1, 1, &nullUav, nullptr);
		deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, renderTargets, dsView, 0, 1, &uav, nullptr);

	}
}

void Graphics::UpdateParticles()
{
	if (NUM_PARTICLES > 9 && NUM_PARTICLES % 10 == 0) {
		deviceContext->Dispatch(NUM_PARTICLES/10, 1, 1);
	}
	else {
		assert(false&& "Not an even number of particles, or not enough particles.");
	}

	deviceContext->CSSetUnorderedAccessViews(1, 1, &nullUav, nullptr);
}

void Graphics::SetProjection(DirectX::XMMATRIX proj)
{
	projection = proj;
}

void Graphics::SetCamera(DirectX::XMMATRIX cam)
{
	camera = cam;
}

DirectX::XMMATRIX Graphics::GetProjection() const
{
	return projection;
}

DirectX::XMMATRIX Graphics::GetCamera() const
{
	return camera;
}

const TextureRT* Graphics::GetGBuffer() const
{
	return gBuffer;
}

ID3D11Texture2D* Graphics::GetBackBuffer() const
{
	ID3D11Texture2D* backbuffer = nullptr;
	if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backbuffer)))) {
		std::cerr << "Failed to get backbuffer.\n";
		assert(false&& "Failed to get back buffer.");
		return nullptr;
	}

	return backbuffer;
}

int Graphics::GetWidth()
{
	return w;
}

int Graphics::GetHeight()
{
	return h;
}

void Graphics::EnableImGui()
{
	imGuiEnabled = true;
}

void Graphics::DisableImGui()
{
	imGuiEnabled = false;
}

bool Graphics::ImGuiEnabled() const
{
	return imGuiEnabled;
}

void Graphics::present()
{
	swapChain->Present(1, 0);
}

void Graphics::SetNormalViewPort()
{
	deviceContext->RSSetViewports(1, &viewport);
}

bool Graphics::IsDebug()
{
	if (shaderDir == "../Debug") {
		return true;
	}
	else {
		return false;
	}
}

std::string Graphics::GetShaderDir()
{
	return this->shaderDir;
}

void Graphics::SetUpImGui(HWND& window)
{
	//Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, deviceContext);
	ImGui::StyleColorsClassic();
	imGuiEnabled = true;
}

void Graphics::ImGuiStart()
{
	if (imGuiEnabled) {
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}
}

void Graphics::ImGuiEnd()
{
	if (imGuiEnabled) {
		deviceContext->OMSetRenderTargets(1, &rtv, dsView);
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
}

bool Graphics::CreateDeviceAndSwapchain(int width, int height, HWND& window)
{
	UINT flags = NULL;
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

	DXGI_SWAP_CHAIN_DESC scDesc = {};
	scDesc.BufferDesc.Width = width;
	scDesc.BufferDesc.Height = height;
	scDesc.BufferDesc.RefreshRate.Numerator = 0;
	scDesc.BufferDesc.RefreshRate.Denominator = 1;
	scDesc.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;

	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
	scDesc.BufferCount = 1;
	scDesc.OutputWindow = window;
	scDesc.Windowed = true;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scDesc.Flags = 0;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, featureLevels, 1, D3D11_SDK_VERSION, &scDesc, &swapChain, &device, nullptr, &deviceContext);
	if (FAILED(hr)) {
		return false;
	}


	return true;
}

bool Graphics::CreateDepthStencilView(int width, int height)
{

	D3D11_TEXTURE2D_DESC dsTextureDesc = {};
	dsTextureDesc.Width = width;
	dsTextureDesc.Height = height;
	dsTextureDesc.MipLevels = 1;
	dsTextureDesc.ArraySize = 1;
	dsTextureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsTextureDesc.SampleDesc.Count = 1;
	dsTextureDesc.SampleDesc.Quality = 0;
	dsTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	dsTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsTextureDesc.CPUAccessFlags = 0;
	dsTextureDesc.MiscFlags = 0;

	if (FAILED(device->CreateTexture2D(&dsTextureDesc, nullptr, &dsTexture))) {
		std::cerr << "Failed to create ds texture.\n";
		return false;
	}


	HRESULT hr = device->CreateDepthStencilView(dsTexture, 0, &dsView);
	assert(!FAILED(hr) && "Failed to create dsview.");



	dsTextureDesc.Width = W_H_CUBE;
	dsTextureDesc.Height = W_H_CUBE;



	if (FAILED(device->CreateTexture2D(&dsTextureDesc, nullptr, &dsTextureCubeMap))) {
		std::cerr << "Failed to create ds texture.\n";
		return false;
	}

	hr = device->CreateDepthStencilView(dsTextureCubeMap, 0, &dsViewCubeMap);

	return !FAILED(hr);
}

void Graphics::SetViewport(int width, int height)
{
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
}

bool Graphics::SetUpSampler(ID3D11Device* device, ID3D11SamplerState*& samState)
{
	D3D11_SAMPLER_DESC sampler = {};
	sampler.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 1;
	

	HRESULT hr = device->CreateSamplerState(&sampler, &samState);


	return !FAILED(hr);
}

bool Graphics::SetUpGbuffer(ID3D11Device* device, int width, int height)
{


	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	for (int i = 0; i < numGbufs; i++) {
		if (FAILED(device->CreateTexture2D(&texDesc, nullptr, &gBuffer[i].texture))) {
			std::cerr << "Failed to create the g texture.\n";
			return false;
		}
	}


	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	for (int i = 0; i < numGbufs; i++) {
		if (FAILED(device->CreateRenderTargetView(gBuffer[i].texture, &rtvDesc, &gBuffer[i].rtv))) {
			std::cerr << "Failed to create the g render target view.\n";
			return false;
		}
	}


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	for (int i = 0; i < numGbufs; i++) {
		if (FAILED(device->CreateShaderResourceView(gBuffer[i].texture, &srvDesc, &gBuffer[i].srv))) {
			std::cerr << "Failed to create the g shader resource view.\n";
			return false;
		}
	}



	return true;
}

bool Graphics::SetUpGbufferCubeMap(ID3D11Device* device, int width, int height)
{
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = W_H_CUBE;
	texDesc.Height = W_H_CUBE;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	for (int i = 0; i < numGbufs; i++) {
		if (FAILED(device->CreateTexture2D(&texDesc, nullptr, &gBufferCubeMap[i].texture))) {
			std::cerr << "Failed to create the g texture.\n";
			return false;
		}
	}


	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	for (int i = 0; i < numGbufs; i++) {
		if (FAILED(device->CreateRenderTargetView(gBufferCubeMap[i].texture, &rtvDesc, &gBufferCubeMap[i].rtv))) {
			std::cerr << "Failed to create the g render target view.\n";
			return false;
		}
	}


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	for (int i = 0; i < numGbufs; i++) {
		if (FAILED(device->CreateShaderResourceView(gBufferCubeMap[i].texture, &srvDesc, &gBufferCubeMap[i].srv))) {
			std::cerr << "Failed to create the g shader resource view.\n";
			return false;
		}
	}



	return true;
}

bool Graphics::CreateUAV(ID3D11Device* device, int width, int height)
{

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;


	ID3D11Texture2D* backbuffer = GetBackBuffer();

	HRESULT hr = device->CreateUnorderedAccessView(backbuffer, &uavDesc, &uav);

	backbuffer->Release();
	return !FAILED(hr);
}

bool Graphics::CreateRTV(ID3D11Device* device, int width, int height)
{
	ID3D11Texture2D* backbuffer = GetBackBuffer();

	HRESULT hr = device->CreateRenderTargetView(backbuffer, NULL, &rtv);

	backbuffer->Release();

	return !FAILED(hr);
}
