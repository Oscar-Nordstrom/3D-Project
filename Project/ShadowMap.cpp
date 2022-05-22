#include "ShadowMap.h"


ShadowMap::ShadowMap(Graphics*& gfx, DirectionalLight* light)
	:gfx(gfx)
{
	dLight = nullptr;
	sLight = nullptr;

	assert(LoadShaders(), "Failed to load shaders.");
	assert(CreateDepthStencil(), "Failed to create ds.");
}

ShadowMap::~ShadowMap()
{
	if (vertexShadowShader)vertexShadowShader->Release();
	if (dsTexture)dsTexture->Release();
	if (dsView)dsView->Release();
}

void ShadowMap::StartFirst(DirectX::XMFLOAT3 pos, int flag)
{
	pos += DirectX::XMFLOAT3(0.0f, 20.0f, 0.0f);
	if (flag == SPOT_LIGHT) {
		//Set cam pos and dir to lights values
	}
	else if (flag == DIRECTIONAL_LIGHT) {
		//Set cam dir to lihgs values and dir to players values + offset
		cam.SetPos(pos);
		cam.SetDir(dLight->direction);
	}
	ID3D11RenderTargetView* nullRtv = NULL;
	gfx->GetContext()->OMSetRenderTargets(1, &nullRtv, dsView);
	gfx->GetContext()->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH, 1, 0);
	gfx->SetCamera(cam.GetMatrix());
	gfx->GetContext()->VSSetShader(vertexShadowShader, nullptr, 0);
	
}

void ShadowMap::EndFirst()
{
}

void ShadowMap::SetDirLight(DirectionalLight* dLight)
{
	this->dLight = dLight;
}

void ShadowMap::SetSpotLight(SpotLight* sLight)
{
	this->sLight = sLight;
}

bool ShadowMap::CreateDepthStencil()
{
	D3D11_TEXTURE2D_DESC dsTextureDesc;
	dsTextureDesc.Width = gfx->GetWidth();
	dsTextureDesc.Height = gfx->GetHeight();
	dsTextureDesc.MipLevels = 1;
	dsTextureDesc.ArraySize = 1;
	//dsTextureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	dsTextureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsTextureDesc.SampleDesc.Count = 1;
	dsTextureDesc.SampleDesc.Quality = 0;
	dsTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	dsTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;// | D3D11_BIND_SHADER_RESOURCE;
	dsTextureDesc.CPUAccessFlags = 0;
	dsTextureDesc.MiscFlags = 0;

	if (FAILED(gfx->GetDevice()->CreateTexture2D(&dsTextureDesc, nullptr, &dsTexture))) {
		std::cerr << "Failed to create ds texture.\n";
		return false;
	}

	/*D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc = {};
	dsDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsDesc.Flags = 0;
	dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;*/

	HRESULT hr = gfx->GetDevice()->CreateDepthStencilView(dsTexture, 0, &dsView);

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



