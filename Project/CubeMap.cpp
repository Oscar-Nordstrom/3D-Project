#include "CubeMap.h"

CubeMap::CubeMap(Graphics*& gfx)
{
	assert(setUpTextures(gfx->GetDevice())&& "Failed to set up textures.");
	assert(SetUpSrvs(gfx->GetDevice()) && "Failed to set up shader resource view.");
	//assert(SetUpRtvs(gfx->GetDevice()), "Failed to set up render target view.");
	assert(SetUpUavs(gfx->GetDevice()) && "Failed to set up unorderd access view.");
	assert(CreateDepthStencilView(gfx->GetDevice()) && "Failed to set up depth stencil view.");
	assert(LoadShader(gfx) && "Failed to load pixel shader.");
	SetViewport();

	pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	//cam.SetPosition(pos);
	//cam.SetRotationRad(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	cam.SetPosition(0.0f, 0.0f, -3.0f);
	cam.SetProj(90.0f, 100.0f,100.0f, 0.1f, 1000.0f);

	//float fov = 90.0f; //90 degrees field of view
	//float fovRadius = (fov / 360.0f) * DirectX::XM_2PI;//vertical field of view angle in radians
	//float aspectRatio = 1.0f;//static_cast<float>(W_H_CUBE) / static_cast<float>(W_H_CUBE);//The aspect ratio
	//float nearZ = 0.1f; //Minimum viewing 
	//float farZ = 1000.0f;//Maximum viewing distance
	//proj = DirectX::XMMatrixPerspectiveFovLH(fovRadius, aspectRatio, nearZ, farZ);

	nullSrv = nullptr;
	nullUav = nullptr;

}

CubeMap::~CubeMap()
{
	if (tex)tex->Release();
	if (srv)srv->Release();
	for (int i = 0; i < NUM_TEX; i++) {
		if (rtv[i])rtv[i]->Release();
		if (uav[i])uav[i]->Release();
	}
	if (dsTex)dsTex->Release();
	if (dsView)dsView->Release();
	if (pShader)pShader->Release();
	if (cShader)cShader->Release();
}

void CubeMap::Set(ID3D11DeviceContext* context, int num)
{
	context->RSSetViewports(1, &viewPort);
	context->CSSetShader(cShader, nullptr, 0);
	context->CSSetUnorderedAccessViews(6, 1, &uav[num], nullptr);
}

void CubeMap::SetSeccond(Graphics*& gfx)
{
	//gfx->GetContext()->RSSetViewports(1, &viewPort);
	gfx->GetContext()->CSSetShader(cShader, nullptr, 0);
	gfx->GetContext()->CSSetUnorderedAccessViews(6, 1, &nullUav, nullptr);
	gfx->GetContext()->PSSetShader(pShader, nullptr, 0);
	gfx->GetContext()->PSSetShaderResources(3, 1, &srv);
}

void CubeMap::SetEnd(Graphics*& gfx)
{
	gfx->GetContext()->PSSetShaderResources(3, 1, &nullSrv);
	//gfx->SetNormalViewPort();
}

void CubeMap::Clear(ID3D11DeviceContext* context)
{
	FLOAT col[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	for (int i = 0; i < NUM_TEX; i++) {
		//context->ClearRenderTargetView(rtv[i], col);
		context->ClearUnorderedAccessViewFloat(uav[i], col);
	}
}

Camera& CubeMap::GetCam()
{
	return cam;
}

DirectX::XMMATRIX CubeMap::GetProj()
{
	return proj;
}

bool CubeMap::setUpTextures(ID3D11Device*& device)
{
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = W_H_CUBE;
	texDesc.Height = W_H_CUBE;
	texDesc.ArraySize = NUM_TEX;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.BindFlags = /*D3D11_BIND_RENDER_TARGET |*/ D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
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
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.ArraySize = NUM_TEX;

	HRESULT hr = device->CreateShaderResourceView(tex, &srvDesc, &srv);
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

bool CubeMap::SetUpUavs(ID3D11Device*& device)
{
	HRESULT hr;

	for (int i = 0; i < NUM_TEX; i++) {
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
		uavDesc.Texture2DArray.FirstArraySlice = (UINT)i;
		uavDesc.Texture2DArray.MipSlice = 0;
		uavDesc.Texture2DArray.ArraySize = 1;

		hr = device->CreateUnorderedAccessView(tex, &uavDesc, &uav[i]);
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

bool CubeMap::LoadShader(Graphics*& gfx)
{
	std::string shaderData;
	std::ifstream reader;
	std::string dir = gfx->GetShaderDir();
	reader.open(dir + "/PixelShaderCubeMapTest.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open PS file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);//Go to the end of the file
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));//Reserve space based on the size of the file
	reader.seekg(0, std::ios::beg);//Go to the start of the file

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());//Assign the file to the shader data


	if (FAILED(gfx->GetDevice()->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pShader)))
	{
		std::cerr << "Failed to create pixel shader!" << std::endl;
		return false;
	}



	shaderData.clear();//Clear the string with data
	reader.close();//Close the file

	reader.open(dir +"/ComputeShaderCubeMapTest.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open PS file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);//Go to the end of the file
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));//Reserve space based on the size of the file
	reader.seekg(0, std::ios::beg);//Go to the start of the file

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());//Assign the file to the shader data


	if (FAILED(gfx->GetDevice()->CreateComputeShader(shaderData.c_str(), shaderData.length(), nullptr, &cShader)))
	{
		std::cerr << "Failed to create pixel shader!" << std::endl;
		return false;
	}



	shaderData.clear();//Clear the string with data
	reader.close();//Close the file

	return true;
}

void CubeMap::SetViewport()
{
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = W_H_CUBE;
	viewPort.Height = W_H_CUBE;
	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1;
}
