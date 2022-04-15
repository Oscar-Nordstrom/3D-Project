#include "CubeMap.h"

CubeMap::CubeMap(Graphics*& gfx)
{
	assert(setUpTextures(gfx->GetDevice()), "Failed to set up textures.");
	assert(SetUpSrvs(gfx->GetDevice()), "Failed to set up shader resource view.");
	assert(SetUpRtvs(gfx->GetDevice()), "Failed to set up render target view.");
	assert(CreateDepthStencilView(gfx->GetDevice()), "Failed to set up depth stencil view.");
	assert(LoadShader(gfx->GetDevice()), "Failed to oad pixel shader.");
}

CubeMap::~CubeMap()
{
	if (tex)tex->Release();
	if (srv)srv->Release();
	for (int i = 0; i < NUM_TEX; i++) {
		if (rtv[i])rtv[i]->Release();
	}
	if (dsTex)dsTex->Release();
	if (dsView)dsView->Release();
	if (pShader)pShader->Release();
}

void CubeMap::Set(ID3D11DeviceContext* context, int num)
{
	//Set render targets
	context->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH, 1.0f, 0.0f);
	assert(num >= 0 && num < NUM_TEX, "expexted arg int between 0-5");
	context->OMSetRenderTargets(1, &rtv[num], dsView);
	//Set shaders
	context->PSSetShader(pShader, nullptr, 0);
	context->OMSetRenderTargets(6, rtv, dsView);
}

void CubeMap::Clear(ID3D11DeviceContext* context)
{
	FLOAT col[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	for (int i = 0; i < NUM_TEX; i++) {
		context->ClearRenderTargetView(rtv[i], col);
	}
}

bool CubeMap::setUpTextures(ID3D11Device*& device)
{
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = W_H_CUBE;
	texDesc.Height = W_H_CUBE;
	texDesc.ArraySize = NUM_TEX;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.CPUAccessFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.MipLevels = 1;

	HRESULT hr = device->CreateTexture2D(&texDesc, nullptr, &tex);

	return !FAILED(hr);
}

bool CubeMap::SetUpSrvs(ID3D11Device*& device)
{
	HRESULT hr = device->CreateShaderResourceView(tex, nullptr, &srv);
	return !FAILED(hr);
}

bool CubeMap::SetUpRtvs(ID3D11Device*& device)
{
	HRESULT hr;
	for (int i = 0; i < NUM_TEX; i++) {
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		rtvDesc.Texture2DArray.FirstArraySlice = (UINT)i;
		rtvDesc.Texture2DArray.ArraySize = 1;

		hr = device->CreateRenderTargetView(tex, &rtvDesc, &rtv[i]);
		if (FAILED(hr)) {
			return false;
		}
	}
	
	return true;
}

bool CubeMap::CreateDepthStencilView(ID3D11Device*& device)
{
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = W_H_CUBE;
	texDesc.Height = W_H_CUBE;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_D32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	HRESULT hr = device->CreateTexture2D(&texDesc, nullptr, &dsTex);
	if (FAILED(hr)) {
		return false;
	}
	hr = device->CreateDepthStencilView(dsTex, nullptr, &dsView);
	return !FAILED(hr);
}

bool CubeMap::LoadShader(ID3D11Device*& device)
{
	std::string shaderData;
	std::ifstream reader;
	reader.open("../Debug/PixelShaderCubeMap.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open PS file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);//Go to the end of the file
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));//Reserve space based on the size of the file
	reader.seekg(0, std::ios::beg);//Go to the start of the file

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());//Assign the file to the shader data

	
	if (FAILED(device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pShader)))
	{
		std::cerr << "Failed to create pixel shader!" << std::endl;
		return false;
	}
	


	shaderData.clear();//Clear the string with data
	reader.close();//Close the file

	return true;
}
