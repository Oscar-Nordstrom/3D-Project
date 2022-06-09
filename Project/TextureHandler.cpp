#include "TextureHandler.h"

TextureHandler::TextureHandler()
{
	imgCheck = 0;
}

TextureHandler::~TextureHandler()
{
}

void TextureHandler::Delete()
{
	int djkwdi = 0;
	for (int i = 0; i < images.size(); i++) {
		if (images[i].tex) {
			images[i].tex->Release();
			delete images[i].img;
		}
	}
}

bool TextureHandler::AddTexture(std::string name, ID3D11Device*& device)
{
	ImageData data;
	data.name = name;
	std::string file = "../Resources/Textures/" + name;
	data.img = stbi_load(file.c_str(), &data.width, &data.height, &data.channels, STBI_rgb_alpha);
	if (data.img == nullptr) {
		return false;
	}
	

	D3D11_TEXTURE2D_DESC text2D;
	text2D.Width = data.width;
	text2D.Height = data.height;
	text2D.MipLevels = 1;
	text2D.ArraySize = 1;
	text2D.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	text2D.SampleDesc.Count = 1;
	text2D.SampleDesc.Quality = 0;
	text2D.Usage = D3D11_USAGE_IMMUTABLE;
	text2D.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	text2D.CPUAccessFlags = 0;
	text2D.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA text2dData;
	text2dData.pSysMem = data.img;
	text2dData.SysMemPitch = data.width * 4;
	text2dData.SysMemSlicePitch = 0;

	//Create the texture
	HRESULT hr = device->CreateTexture2D(&text2D, &text2dData, &data.tex);
	if (FAILED(hr)) {
		return false;
	}

	images.push_back(data);
	return true;
}

bool TextureHandler::TextureExists(std::string name)
{
	for (int i = 0; i < images.size(); i++) {
		if (images[i].name == name) {
			return true;
		}
	}
	return false;
}

bool TextureHandler::TextureExists(int index)
{
	if (index >= 0 && index < images.size()) {
		return true;
	}
	return false;
}

ImageData TextureHandler::GetImage(std::string name)
{
	for (int i = 0; i < images.size(); i++) {
		if (images[i].name == name) {
			return images[i];
		}
	}
	return images[0];
}

ImageData TextureHandler::GetImage(int index)
{
	if (index >= 0 && index < images.size()) {
		return images[index];
	}
	return images[0];
}

int TextureHandler::NumImages() const
{
	return images.size();
}


void TextureHandler::AddTestImage()
{
	imgCheck++;
}

