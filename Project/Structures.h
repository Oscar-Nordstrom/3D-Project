#pragma once

#include <array>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include <d3d11.h>
#include <fstream>
#include <sstream>
#include <iostream>

#include "stb_image.h"

struct ShadowVertexData {
	DirectX::XMMATRIX proj;
	DirectX::XMMATRIX view;
};

struct DirectionalLight {
	DirectionalLight(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f))
		:direction(dir), color(color)
	{
	}
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT3 direction;
};

struct SpotLight {
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 direction;
	float innerAngle;
	float outerAngle;
};

struct TextureRT {
	ID3D11Texture2D* texture;
	ID3D11RenderTargetView* rtv;
	ID3D11ShaderResourceView* srv;
};

struct MtlImages {
	std::vector<std::string> names;
	std::vector<unsigned char*> img;
	std::vector<int> widths;
	std::vector<int> heights;

	MtlImages(std::string fileName) {
		std::stringstream ss;
		std::ifstream file;
		std::string line;
		std::string prefix = "";
		std::string temp;

		file.open("../Resources/Mtl/" + fileName);
		if (!file.is_open()) {
			std::cerr << "Failed to open mesh file.\n";
		}
		while (std::getline(file, line)) {
			ss.clear();
			if (line.size() > 0 && line[line.size() - 1] != ' ') {
				line += " ";
			}
			ss.str(line);
			ss >> prefix;
			if (prefix == "map_Kd") {
				ss >> temp;
				names.push_back(prefix);
				AddImg(temp);
			}
			else if (prefix == "map_Ks") {
				ss >> temp;
				names.push_back(prefix);
				AddImg(temp);
			}
			else if (prefix == "map_Ka") {
				ss >> temp;
				names.push_back(prefix);
				AddImg(temp);
			}
		}
	}

	bool AddImg(std::string what) {
		int width, height, channels;
		unsigned char* imgTemp;
		std::string file = "../Resources/Mtl/" + what;
		imgTemp = stbi_load(file.c_str(), &width, &height, &channels, STBI_rgb_alpha);
		if (imgTemp == nullptr) {
			std::cerr << "Failed to load image.\n";
			return false;
		}
		img.push_back(imgTemp);
		widths.push_back(width);
		heights.push_back(height);
		return true;
	}

};

struct SimpleVertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 n;
	DirectX::XMFLOAT2 uv;

	SimpleVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT2& UV)
	{
		pos = position;
		n = normal;
		uv = UV;
	}
	bool operator==(SimpleVertex& vertex)const {
		if (pos.x == vertex.pos.x && pos.y == vertex.pos.y && pos.z == vertex.pos.z) {
			if (n.x == vertex.n.x && n.y == vertex.n.y && n.z == vertex.n.z) {
				if (uv.x == vertex.uv.x && uv.y == vertex.uv.y) {
					return true;
				}
			}
		}
		return false;
	}
};

struct Matrices
{
	DirectX::XMFLOAT4X4 mOne;
	DirectX::XMFLOAT4X4 mTwo;
	DirectX::XMFLOAT4X4 mThree;

	Matrices(DirectX::XMFLOAT4X4& one, DirectX::XMFLOAT4X4& two, DirectX::XMFLOAT4X4& three)
	{
		mOne = one;
		mTwo = two;
		mThree = three;
	}
};

struct Material {
	float Ns;
	std::string map_Kd;
	std::string map_Ks;
	std::string map_Ka;

	Material(float Ns, std::string map_Kd, std::string map_Ks, std::string map_Ka)
		:Ns(Ns), map_Kd(map_Kd), map_Ks(map_Ks), map_Ka(map_Ka)
	{
	}
	int roundUpTo(int numToRound, int multiple)
	{
		if (multiple == 0)
			return numToRound;

		int remainder = numToRound % multiple;
		if (remainder == 0)
			return numToRound;

		return numToRound + multiple - remainder;
	}
	bool SetupTexture(ID3D11Device* device, ID3D11Texture2D* textureBuffer[], MtlImages* mtlFileTex) {

		bool found = false;
		bool found1 = false;
		bool found2 = false;
		int one, two, three;
		int i = 0;
		int size = mtlFileTex->names.size();
		for (i = 0; i < size; i++) {
			if (mtlFileTex->names[i] == "map_Kd") {
				found = true;
				one = i;
			}
			else if (mtlFileTex->names[i] == "map_Ks") {
				found1 = true;
				two = i;
			}
			else if (mtlFileTex->names[i] == "map_Ka") {
				found2 = true;
				three = i;
			}
			if (found && found1 && found2) {
				break;
			}
		}
		if (!found || !found1 || !found2) {
			return false;
		}



		D3D11_TEXTURE2D_DESC text2D;
		text2D.Width = mtlFileTex->widths[one];
		text2D.Height = mtlFileTex->heights[one];
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
		text2dData.pSysMem = mtlFileTex->img[one];
		text2dData.SysMemPitch = mtlFileTex->widths[one] * 4;
		text2dData.SysMemSlicePitch = 0;

		//Create the texture
		if (FAILED(device->CreateTexture2D(&text2D, &text2dData, &textureBuffer[0]))) {
			return false;
		}

		text2D.Width = mtlFileTex->widths[two];
		text2D.Height = mtlFileTex->heights[two];
		text2D.MipLevels = 1;
		text2D.ArraySize = 1;
		text2D.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		text2D.SampleDesc.Count = 1;
		text2D.SampleDesc.Quality = 0;
		text2D.Usage = D3D11_USAGE_IMMUTABLE;
		text2D.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		text2D.CPUAccessFlags = 0;
		text2D.MiscFlags = 0;

		text2dData.pSysMem = mtlFileTex->img[two];
		text2dData.SysMemPitch = mtlFileTex->widths[two] * 4;
		text2dData.SysMemSlicePitch = 0;

		//Create the texture
		if (FAILED(device->CreateTexture2D(&text2D, &text2dData, &textureBuffer[1]))) {
			return false;
		}

		text2D.Width = mtlFileTex->widths[three];
		text2D.Height = mtlFileTex->heights[three];
		text2D.MipLevels = 1;
		text2D.ArraySize = 1;
		text2D.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		text2D.SampleDesc.Count = 1;
		text2D.SampleDesc.Quality = 0;
		text2D.Usage = D3D11_USAGE_IMMUTABLE;
		text2D.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		text2D.CPUAccessFlags = 0;
		text2D.MiscFlags = 0;

		text2dData.pSysMem = mtlFileTex->img[three];
		text2dData.SysMemPitch = mtlFileTex->widths[three] * 4;
		text2dData.SysMemSlicePitch = 0;

		//Create the texture
		if (FAILED(device->CreateTexture2D(&text2D, &text2dData, &textureBuffer[2]))) {
			return false;
		}

		return true;
	}
	bool CreateSRV(ID3D11Device* device, ID3D11Texture2D* textureBuffer[], ID3D11ShaderResourceView* srv[]) {
		if (FAILED(device->CreateShaderResourceView(textureBuffer[0], nullptr, &srv[0]))) {
			return false;
		}
		if (FAILED(device->CreateShaderResourceView(textureBuffer[1], nullptr, &srv[1]))) {
			return false;
		}
		if (FAILED(device->CreateShaderResourceView(textureBuffer[2], nullptr, &srv[2]))) {
			return false;
		}
		return true;
	}
	bool SetUpCbuf(ID3D11Device* device, ID3D11Buffer*& cbuf) {
		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = roundUpTo(sizeof(Ns), 16);
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = &Ns;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		HRESULT hr = device->CreateBuffer(&desc, &data, &cbuf);

		return !FAILED(hr);
	}
};

struct SubMesh {
	std::string name;
	std::string mtlFile;
	std::string mtl;
	int start;
	int end;

	std::vector<unsigned short> subIndices;
	ID3D11Buffer* indexBuffer;
	ID3D11Texture2D* textureBuffer[3];
	ID3D11ShaderResourceView* srv[3];
	ID3D11Buffer* cbuf;

	SubMesh(ID3D11Device* device, ID3D11DeviceContext* context, std::vector<unsigned short>& indices, MtlImages* mtlFileTex, std::string name, std::string mtlFile, std::string mtl, int start, int end)
		:name(name), mtlFile(mtlFile), mtl(mtl), start(start), end(end)
	{
		SetUpIndexBuffer(device, indices);
		if (!LoadMtl(device, context, mtlFileTex, mtlFile)) {
			std::cerr << "Could not load mtl file.\n";
		}
	}

	bool SetUpIndexBuffer(ID3D11Device* device, std::vector<unsigned short>& indices) {
		int size = indices.size();
		for (int i = 0; i < size; i++) {
			if (i >= start && i <= end) {
				subIndices.push_back(indices[i]);
			}
		}

		D3D11_BUFFER_DESC ibDesc;
		ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibDesc.Usage = D3D11_USAGE_DEFAULT;
		ibDesc.CPUAccessFlags = 0;
		ibDesc.MiscFlags = 0;
		ibDesc.ByteWidth = UINT(subIndices.size() * sizeof(unsigned short));
		ibDesc.StructureByteStride = sizeof(unsigned short);

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = subIndices.data();
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		HRESULT hr = device->CreateBuffer(&ibDesc, &data, &indexBuffer);

		return !FAILED(hr);

	}
	bool LoadMtl(ID3D11Device* device, ID3D11DeviceContext* context, MtlImages* mtlFileTex, std::string fileName) {

		float Ns = 0.0f;

		std::string map_Kd = "";
		std::string map_Ks = "";
		std::string map_Ka = "";

		//Read the mtl file
		std::stringstream ss;
		std::ifstream file;
		std::string line;
		std::string prefix = "";
		std::string check;
		bool found = false;
		file.open("../Resources/Mtl/" + fileName);
		if (!file.is_open()) {
			std::cerr << "Failed to open mesh file.\n";
			return false;
		}
		while (std::getline(file, line)) {
			ss.clear();
			if (line.size() > 0 && line[line.size() - 1] != ' ') {
				line += " ";
			}
			ss.str(line);
			ss >> prefix;
			if (prefix == "newmtl") {
				ss >> check;
				if (!found) {
					if (check == mtl) {
						found = true;
					}
				}

			}
			if (found) {
				if (prefix == "Ns") {

					ss >> Ns;
				}
				/*else if (prefix == "Ka") {
					ss >> Ka.x >> Ka.y >> Ka.z;

				}
				else if (prefix == "Kd") {
					ss >> Kd.x >> Kd.y >> Kd.z;
				}
				else if (prefix == "Ks") {
					ss >> Ks.x >> Ks.y >> Ks.z;
				}
				else if (prefix == "Ke") {
					ss >> Ke.x >> Ke.y >> Ke.z;
				}
				else if (prefix == "Ni") {
					ss >> Ni;
				}
				else if (prefix == "d") {
					ss >> d;
				}
				else if (prefix == "illum") {
					ss >> lum;
				}
				else if (prefix == "map_Bump") {
					ss >> map_Bump;
				}*/
				else if (prefix == "map_Kd") {
					ss >> map_Kd;
				}
				else if (prefix == "map_Ks") {
					ss >> map_Ks;
				}
				else if (prefix == "map_Ka") {
					ss >> map_Ka;
				}
			}
		}
		Material mat(Ns, map_Kd, map_Ks, map_Ka);
		if (!mat.SetupTexture(device, textureBuffer, mtlFileTex)) {
			return false;
		}
		if (!mat.CreateSRV(device, textureBuffer, srv)) {
			return false;
		}
		if (!mat.SetUpCbuf(device, cbuf)) {
			return false;
		}

		context->PSSetShaderResources(0, 3, srv);
		context->CSSetConstantBuffers(0, 1, &cbuf);

		return true;
	}

	void Bind(ID3D11DeviceContext*& context, bool withPS = true) {
		if (withPS)
			context->PSSetShaderResources(0, 3, srv);
		context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

		context->DrawIndexed((UINT)subIndices.size(), 0, 0);

	}

	void Terminate() {
		indexBuffer->Release();
		cbuf->Release();
		for (int i = 0; i < 3; i++) {
			textureBuffer[i]->Release();
			srv[i]->Release();
		}
	}
};

