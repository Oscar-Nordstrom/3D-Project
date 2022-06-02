#include "ShadowMap.h"


ShadowMap::ShadowMap()
{
	this->gfx = nullptr;
	dLight = nullptr;
	sLight1 = nullptr;
	sLight2 = nullptr;
	sLight3 = nullptr;

	lightTurn = 0;
	projection = DirectX::XMMatrixOrthographicLH(200.0f, 200.0f, 0.1f, 100.0f);
}

ShadowMap::~ShadowMap()
{
	for (auto p : dsViews) {
		if (p)p->Release();
	}
	if (dsTexture)dsTexture->Release();
	if (vertexShadowShader)vertexShadowShader->Release();
	if (shadowSRV)shadowSRV->Release();
}

void ShadowMap::Init(Graphics*& gfx, DirectionalLight* light)
{
	this->gfx = gfx;

	assert(LoadShaders() && "Failed to load shaders.");
	assert(CreateDepthStencil() && "Failed to create ds.");
	SetViewPort();
}

void ShadowMap::StartFirst()
{
	for (int i = 0; i < NUM_LIGHTS; i++) {
		gfx->GetContext()->ClearDepthStencilView(this->dsViews[i], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	}
	gfx->GetContext()->RSSetViewports(1, &shadowViewPort);
	gfx->GetContext()->VSSetShader(vertexShadowShader, nullptr, 0);
	ID3D11ShaderResourceView* const pSRV[4] = { NULL,NULL,NULL,NULL };
	gfx->GetContext()->PSSetShaderResources(1, 4, pSRV);
	gfx->GetContext()->PSSetShader(nullptr, nullptr, 0);
	//if (shadowSRV)shadowSRV->Release();
}

void ShadowMap::EndFirst()
{
}

void ShadowMap::StartSeccond()
{
	DepthToSRV();
	gfx->GetContext()->PSSetShaderResources(3, 1, &shadowSRV);
}

void ShadowMap::EndSeccond()
{
}

void ShadowMap::UpdateWhatShadow(int whatLight, int flag)
{
	ID3D11RenderTargetView* pNullRTV = NULL;
	gfx->GetContext()->OMSetRenderTargets(1, &pNullRTV, GetDsView(whatLight));

	if (flag == DIRECTIONAL_LIGHT) {
		//projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(90), static_cast<float>(gfx->GetWidth()) / static_cast<float>(gfx->GetHeight()), 0.1f, 40000.f);
		projection = DirectX::XMMatrixOrthographicLH(200.0f, 200.0f, 0.1f, 100.0f);
	}
	else if (flag == SPOT_LIGHT) {
		switch (whatLight)
		{
		case 1:
			projection = DirectX::XMMatrixPerspectiveFovLH(50.0f, 1.0f, 0.1f, 100.f);
			//projection = DirectX::XMMatrixOrthographicLH(200.0f, 200.0f, 0.1f, 100.0f);
			break;
		case 2:
			projection = DirectX::XMMatrixPerspectiveFovLH(50.0f, 1.0f, 0.1f, 100.f);
			//projection = DirectX::XMMatrixOrthographicLH(200.0f, 200.0f, 0.1f, 100.0f);
			break;
		case 3:
			projection = DirectX::XMMatrixPerspectiveFovLH(50.0f, 1.0f, 0.1f, 100.f);
			//projection = DirectX::XMMatrixOrthographicLH(200.0f, 200.0f, 0.1f, 100.0f);
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

ID3D11ShaderResourceView*& ShadowMap::GetShadowSRV()
{
	return shadowSRV;
}

ID3D11ShaderResourceView*& ShadowMap::DepthToSRV()
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2DArray.ArraySize = NUM_LIGHTS;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = 1;
	HRESULT hr;
	
	if (shadowSRV)shadowSRV->Release();
	hr = gfx->GetDevice()->CreateShaderResourceView(dsTexture, &srvDesc, &shadowSRV);
	assert(!FAILED(hr) && "Failed to convert dsView to SRV");
	return shadowSRV;
	/*for (int i = 0; i < NUM_LIGHTS; i++) {
		dsViews[i]->GetResource(&shadowRes);
		hr = gfx->GetDevice()->CreateShaderResourceView(shadowRes, &srvDesc, &shadowSRV);
		assert(!FAILED(hr)&& "Failed to convert dsView to SRV");
		shadowRes->Release();
	}
	return shadowSRV;*/
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
	texDesc.ArraySize = NUM_LIGHTS;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;


	if (FAILED(gfx->GetDevice()->CreateTexture2D(&texDesc, nullptr, &dsTexture)))
	{
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsViewDesc = {};
	dsViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsViewDesc.Flags = 0;
	dsViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	dsViewDesc.Texture2DArray.ArraySize = 1;
	dsViewDesc.Texture2DArray.MipSlice = 0;

	for (int i = 0; i < NUM_LIGHTS; i++) {
		dsViewDesc.Texture2DArray.FirstArraySlice = D3D11CalcSubresource(0, i, 1);
		ID3D11DepthStencilView* dsView;
		if (FAILED(gfx->GetDevice()->CreateDepthStencilView(dsTexture, &dsViewDesc, &dsView))) {
			return false;
		}
		dsViews.push_back(dsView);
	}

	//DEPTH STENCIL TO SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.ArraySize = NUM_LIGHTS;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = 1;

	HRESULT hr = gfx->GetDevice()->CreateShaderResourceView(dsTexture, &srvDesc, &shadowSRV);

	return !FAILED(hr);
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



