#pragma once
#include "TextureHandler.h"

#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <DirectXMath.h>

#include "SubMesh.h"
#include "Graphics.h"
#include "flags.h"

class FileLoader {
public:
	FileLoader();
	~FileLoader();
	bool LoadObj(std::string filePath, Graphics* gfx);
	void SetTexHandl(TextureHandler*& texHandl);
	std::vector<SimpleVertex> GetVerts();
	std::vector<unsigned short> GetIndices();
	std::vector<SubMesh*> GetSubs();
private:
	bool AddImage(std::string what, Graphics* gfx);
	bool LoadImages(std::string filePath, Graphics* gfx);
	std::vector<SimpleVertex> verts;
	std::vector<unsigned short> indices;
	std::vector<SubMesh*> subs;
	TextureHandler* texHandl;


	std::vector<std::string> names;
	std::vector<ID3D11Texture2D*>textures;

};