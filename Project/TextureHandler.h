#pragma once
#include <vector>
#include <string>
#include <d3d11.h>
#include "stb_image.h"

#include "Structures.h"

class TextureHandler {
public:
	TextureHandler();
	~TextureHandler();
	void Delete();
	bool AddTexture(std::string name, ID3D11Device*& device);
	bool TextureExists(std::string name);
	bool TextureExists(int index);
	ImageData GetImage(std::string name);
	ImageData GetImage(int index);
	int NumImages()const;

	void AddTestImage();
private:
	std::vector<ImageData> images;
	int imgCheck;
};