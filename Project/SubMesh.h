#pragma once

#include <string>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Graphics.h"
#include "TextureHandler.h"
#include "Material.h"

class SubMesh {
public:
	SubMesh(Graphics*& gfx, std::vector<unsigned short>& indices, TextureHandler*& texHandl, std::string name, std::string mtlFile, std::string mtl, int start, int end);
	~SubMesh();
	void Bind(int flag = NORMAL);
private:
	bool SetUpIndexBuffer(std::vector<unsigned short>& indices);
	bool LoadMtl();

	std::string name;
	std::string mtlFile;
	std::string mtl;
	int start;
	int end;

	Graphics* gfx;
	TextureHandler* texHandl;

	std::vector<unsigned short> subIndices;
	ID3D11Buffer* indexBuffer;
	ID3D11ShaderResourceView* srv[3];
	ID3D11Buffer* cbuf;


};