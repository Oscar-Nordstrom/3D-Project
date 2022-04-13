#include "ShadowMap.h"


ShadowMap::ShadowMap(Graphics*& gfx, DirectionalLight* light)
	:gfx(gfx), light(light)
{
	cam.SetDir(light->direction);
	cam.SetPos(DirectX::XMFLOAT3(0.0f, 10.0f, 0.0f));

	assert(LoadShaders(), "Failed to load shaders.");
	assert(CreateDepthStencil(), "Failed to create ds.");
	assert(CreateSRV(), "Failed to create srv.");
	assert(CreateConstantBuffer(), "Failed create constant buffer.");
	/*if (!LoadShaders()) {
		std::cerr << "Failed load shader.\n";
	}
	if (!CreateDepthStencil()) {
		std::cerr << "Failed to create ds.\n";
	}
	if (!CreateSRV()) {
		std::cerr << "Failed to create srv.\n";
	}
	if (!CreateConstantBuffer()) {
		std::cerr << "Failed create constant buffer.\n";
	}*/
}

ShadowMap::~ShadowMap()
{
	if (vertexShadowShader)
		vertexShadowShader->Release();
	if (dsTexture)
		dsTexture->Release();
	if (dsView)
		dsView->Release();
	if (srv)
		srv->Release();
	if (constantBuffer)
		constantBuffer->Release();
}

void ShadowMap::SetCamPos(DirectX::XMFLOAT3 pos)
{
	cam.SetPos(pos);
}

void ShadowMap::SetCamDir(DirectX::XMFLOAT3 dir)
{
	if (dir.x == 0.0f)
		dir.x = 0.00000001f;
	if (dir.y == 0.0f)
		dir.y = 0.00000001f;
	//if (dir.z == 0.0f)
		//dir.z = 0.01f;
	cam.SetDir(dir);
}

void ShadowMap::SetShadowMap()
{
	ID3D11ShaderResourceView* nullSRV = NULL;
	gfx->GetContext()->PSSetShaderResources(3, 1, &nullSRV);
	ID3D11RenderTargetView* nullRTV = NULL;
	gfx->GetContext()->OMSetRenderTargets(1, &nullRTV, dsView);

	gfx->GetContext()->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH, 1, 0);

	gfx->GetContext()->VSSetShader(vertexShadowShader, nullptr, 0);
	gfx->GetContext()->PSSetShader(nullptr, nullptr, 0);

	float nearZ = 0.1f; //Minimum viewing 
	float farZ = 1000.0f;//Maximum viewing distance

	projection = DirectX::XMMatrixOrthographicLH(8, 6, nearZ, farZ);
	gfx->SetProjection(projection);
	gfx->SetCamera(cam.GetMatrix());
}

void ShadowMap::BindDepthResource()
{
	if (!UpdateConstantBuffer()) {
		std::cerr << "Failed update constant buffer.\n";
	}
	gfx->GetContext()->VSSetConstantBuffers(1, 1, &constantBuffer);
	gfx->GetContext()->DSSetConstantBuffers(1, 1, &constantBuffer);
	gfx->GetContext()->PSSetShaderResources(3, 1, &srv);
}

bool ShadowMap::CreateDepthStencil()
{
	D3D11_TEXTURE2D_DESC dsTextureDesc;
	dsTextureDesc.Width = gfx->GetWidth();
	dsTextureDesc.Height = gfx->GetHeight();
	dsTextureDesc.MipLevels = 1;
	dsTextureDesc.ArraySize = 1;
	dsTextureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	dsTextureDesc.SampleDesc.Count = 1;
	dsTextureDesc.SampleDesc.Quality = 0;
	dsTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	dsTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	dsTextureDesc.CPUAccessFlags = 0;
	dsTextureDesc.MiscFlags = 0;


	if (FAILED(gfx->GetDevice()->CreateTexture2D(&dsTextureDesc, nullptr, &dsTexture))) {
		std::cerr << "Failed to create ds texture.\n";
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc = {};
	dsDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsDesc.Flags = 0;
	dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsDesc.Texture2D.MipSlice = 0;

	HRESULT hr = gfx->GetDevice()->CreateDepthStencilView(dsTexture, &dsDesc, &dsView);

	return !FAILED(hr);
}

bool ShadowMap::LoadShaders()
{
	std::string shaderData;
	std::ifstream reader;
	//Open the vertex shader cso file
	reader.open("../Debug/VertexShadowShader.cso", std::ios::binary | std::ios::ate);
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

bool ShadowMap::CreateSRV()
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	HRESULT hr = gfx->GetDevice()->CreateShaderResourceView(dsTexture, &srvDesc, &srv);

	return !FAILED(hr);
}

bool ShadowMap::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.ByteWidth = sizeof(ShadowVertexData);
	cbDesc.StructureByteStride = 0;

	ShadowVertexData d;
	d.proj = projection;
	d.view = cam.GetMatrix();

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &d;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = gfx->GetDevice()->CreateBuffer(&cbDesc, &data, &constantBuffer);

	return !FAILED(hr);
}

bool ShadowMap::UpdateConstantBuffer()
{
	
		ShadowVertexData d;
		d.proj = projection;
		d.view = cam.GetMatrix();
		
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));//Clear the mappedResource
		HRESULT hr = gfx->GetContext()->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &d, sizeof(ShadowVertexData));//Write the new memory 
		
		gfx->GetContext()->Unmap(constantBuffer, 0);
		if (FAILED(hr)) {
			assert(false, "Failed to update constant buffer.");
		}

		return true;
}


