#pragma once
#include <vector>
#include <string>
#include <d3d11.h>
#include "stb_image.h"

struct ImageData {
	std::string name;
	int width, height, channels;
	unsigned char* img;
	ID3D11Texture2D* tex;
};

class TextureHandler {
public:
	TextureHandler();
	~TextureHandler();
	void Delete();
	bool AddTexture(std::string name, ID3D11Device*& device);
	bool TextureExists(std::string name);
	ImageData GetImage(std::string name)const;

	void AddTestImage();
private:
	std::vector<ImageData> images;
	int imgCheck;
};