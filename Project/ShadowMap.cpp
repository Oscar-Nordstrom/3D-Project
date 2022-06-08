#include "ShadowMap.h"


ShadowMap::ShadowMap()
{
	this->gfx = nullptr;
	dLight = nullptr;
	sLight1 = nullptr;
	sLight2 = nullptr;
	sLight3 = nullptr;
	shadowRes = nullptr;
	vertexShadowShader = nullptr;
	samState = nullptr;

	for (int i = 0; i < 4; i++) {
		dsViews[i] = nullptr;
		dsTexture[i] = nullptr;
		shadowSRV[i] = nullptr;
	}

	lightTurn = 0;
	projection = DirectX::XMMatrixOrthographicLH(200.0f, 200.0f, 0.1f, 100.0f);
}

ShadowMap::~ShadowMap()
{
	if (samState)samState->Release();
	if (vertexShadowShader)vertexShadowShader->Release();

	for (int i = 0; i < 4; i++) {
		if (dsViews[i])dsViews[i]->Release();
		if (shadowSRV[i])shadowSRV[i]->Release();
		if (dsTexture[i])dsTexture[i]->Release();
	}
}

void ShadowMap::Init(Graphics*& gfx, DirectionalLight* light)
{
	this->gfx = gfx;

	assert(LoadShaders() && "Failed to load shaders.");
	assert(CreateDepthStencil() && "Failed to create ds.");
	assert(SetUpSampler() && "Failed to set up sampler.");
	SetViewPort();
}

void ShadowMap::StartFirst()
{
	for (int i = 0; i < NUM_LIGHTS; i++) {
		gfx->GetContext()->ClearDepthStencilView(this->dsViews[i], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	}
	gfx->GetContext()->RSSetViewports(1, &shadowViewPort);
	gfx->GetContext()->VSSetShader(vertexShadowShader, nullptr, 0);
	ID3D11ShaderResourceView* const pSRV[7] = { NULL };
	gfx->GetContext()->PSSetShaderResources(1, 7, pSRV);
	gfx->GetContext()->PSSetShader(nullptr, nullptr, 0);
}

void ShadowMap::EndFirst()
{
}

void ShadowMap::StartSeccond()
{
	DepthToSRV();
	gfx->GetContext()->PSSetShaderResources(3, 1, &shadowSRV[0]);
	gfx->GetContext()->PSSetShaderResources(4, 1, &shadowSRV[1]);
	gfx->GetContext()->PSSetShaderResources(5, 1, &shadowSRV[2]);
	gfx->GetContext()->PSSetShaderResources(6, 1, &shadowSRV[3]);
	gfx->GetContext()->PSSetSamplers(1, 1, &samState);
}

void ShadowMap::EndSeccond()
{
}

void ShadowMap::UpdateWhatShadow(int whatLight, int flag)
{
	ID3D11RenderTargetView* pNullRTV = NULL;
	gfx->GetContext()->OMSetRenderTargets(1, &pNullRTV, GetDsView(whatLight));

	if (flag == DIRECTIONAL_LIGHT) {
		projection = DirectX::XMMatrixOrthographicLH(200.0f, 200.0f, 0.1f, 100.0f);
	}
	else if (flag == SPOT_LIGHT) {
		switch (whatLight)
		{
		case 1:
			projection = DirectX::XMMatrixPerspectiveFovLH(sLight1->outerAngle, 1.0f, 0.1f, 100.f);
			break;
		case 2:
			projection = DirectX::XMMatrixPerspectiveFovLH(sLight2->outerAngle, 1.0f, 0.1f, 100.f);
			break;
		case 3:
			projection = DirectX::XMMatrixPerspectiveFovLH(sLight3->outerAngle, 1.0f, 0.1f, 100.f);
			break;
		}
	}
	else {
		assert(false && "Need a spot light or a direction light.");
	}

	gfx->SetProjection(projection);
}

void ShadowMap::SetDirLight(DirectionalLight* dLight)
{
	this->dLight = dLight;
}

void ShadowMap::SetSampler(int slot)
{
	gfx->GetContext()->PSSetSamplers(slot, 1, &samState);
}

void ShadowMap::SetSpotLights(SpotLight spotLights[])
{
	this->sLight1 = &spotLights[0];
	this->sLight2 = &spotLights[1];
	this->sLight3 = &spotLights[2];
}

ID3D11DepthStencilView* ShadowMap::GetDsView(int what)
{
	if (what < 0 || what > 3) {
		assert(false && "Failed to get dsView, number out of range. Use value between 0-3");
	}
	return dsViews[what];
}

void ShadowMap::DepthToSRV()
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2DArray.ArraySize = 1;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = 1;
	HRESULT hr;

	for (int i = 0; i < 4; i++) {
		if (shadowSRV[i])shadowSRV[i]->Release();
		dsViews[i]->GetResource(&shadowRes);
		hr = gfx->GetDevice()->CreateShaderResourceView(shadowRes, &srvDesc, &shadowSRV[i]);
		assert(!FAILED(hr) && "Failed to convert dsView to SRV");
		shadowRes->Release();
	}
}

void ShadowMap::SetViewPort()
{
	shadowViewPort.TopLeftX = 0;
	shadowViewPort.TopLeftY = 0;
	shadowViewPort.Width = static_cast<float>(gfx->GetWidth());
	shadowViewPort.Height = static_cast<float>(gfx->GetHeight());
	shadowViewPort.MinDepth = 0;
	shadowViewPort.MaxDepth = 1;
}

bool ShadowMap::CreateDepthStencil()
{
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = gfx->GetWidth();
	texDesc.Height = gfx->GetHeight();
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	for (int i = 0; i < 4; i++) {
		if (FAILED(gfx->GetDevice()->CreateTexture2D(&texDesc, nullptr, &dsTexture[i])))
		{
			return false;
		}
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsViewDesc = {};
	dsViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsViewDesc.Flags = 0;
	dsViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	dsViewDesc.Texture2DArray.ArraySize = 1;
	dsViewDesc.Texture2DArray.MipSlice = 0;

	for (int i = 0; i < 4; i++) {
		if (FAILED(gfx->GetDevice()->CreateDepthStencilView(dsTexture[i], &dsViewDesc, &dsViews[i]))) {
			return false;
		}
	}

	//DEPTH STENCIL TO SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	HRESULT hr;
	for (int i = 0; i < 4; i++) {
		hr = gfx->GetDevice()->CreateShaderResourceView(dsTexture[i], &srvDesc, &shadowSRV[i]);
		assert(!FAILED(hr) && "Failed to create srv");
	}

	return true;
}

bool ShadowMap::LoadShaders()
{
	std::string shaderData;
	std::ifstream reader;
	//Open the vertex shader cso file
	reader.open(gfx->GetShaderDir() + "/VertexShadowShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open VS file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);//Go to the end of the file
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));//Reserve space based on how big the file is
	reader.seekg(0, std::ios::beg);//Go the the start of the file

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>()); //Assign the file to the shaderdata

	//Create our vertwx shader and store it in vShader
	if (FAILED(gfx->GetDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &vertexShadowShader)))
	{
		std::cerr << "Failed to create vertex shader!" << std::endl;
		return false;
	}


	shaderData.clear();//Clear the string with data
	reader.close();//Close the file


	return true;
}

bool ShadowMap::SetUpSampler()
{
	D3D11_SAMPLER_DESC sampler;
	sampler.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler.BorderColor[0] = 1;
	sampler.BorderColor[1] = 1;
	sampler.BorderColor[2] = 1;
	sampler.BorderColor[3] = 1;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 1;

	HRESULT hr = gfx->GetDevice()->CreateSamplerState(&sampler, &samState);

	return !FAILED(hr);
}



