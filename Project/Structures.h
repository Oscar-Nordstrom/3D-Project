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

#include "flags.h"

#include "TextureHandler.h"

struct TimeData {
	float dt;
	float time;
};

struct ShadowShaderBuffer {
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX proj;
};

struct DirectionalLight {
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT3 direction;
};

struct SpotLight {
	DirectX::XMFLOAT4 color;//16 bytes
	DirectX::XMFLOAT3 position;//12 bytes
	float innerAngle;//4 bytes
	DirectX::XMFLOAT3 direction;//12 bytes
	float outerAngle;//4 bytes
};

struct TextureRT {
	ID3D11Texture2D* texture;
	ID3D11RenderTargetView* rtv;
	ID3D11ShaderResourceView* srv;
};

struct MtlData {
	float Ns;
	DirectX::XMFLOAT3 kd, ks, ka;
};

struct MtlImages {
	std::vector<std::string> names;
	std::vector<ID3D11Texture2D*>textures;
	TextureHandler* texHandl;

	MtlImages(std::string fileName, ID3D11Device*& device, TextureHandler*& texHandl) {
		this->texHandl = texHandl;
		std::stringstream ss;
		std::ifstream file;
		std::string line;
		std::string prefix = "";
		std::string temp;

		
		if (AddImg("Default.png", device)) {
			names.push_back("Default");
		};
		texHandl->AddTestImage();

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
				names.push_back(temp);
				AddImg(temp, device);
			}
			else if (prefix == "map_Ks") {
				ss >> temp;
				names.push_back(temp);
				AddImg(temp, device);
			}
			else if (prefix == "map_Ka") {
				ss >> temp;
				names.push_back(temp);
				AddImg(temp, device);
			}
		}

	}
	~MtlImages() {

	}
	bool AddImg(std::string what, ID3D11Device*& device) {
		ImageData data;
		if (!texHandl->TextureExists(what)) {
			if (!texHandl->AddTexture(what, device)) {
				return false;
			}
			texHandl->AddTestImage();
			data = texHandl->GetImage(what);
		}
		else {
			data = texHandl->GetImage(what);
		}
		
		textures.push_back(data.tex);

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
	std::string make_this_string() {
		std::string ret;

		ret = std::to_string(pos.x)+ std::to_string(pos.y) + std::to_string(pos.z) + std::to_string(n.x) + std::to_string(n.y) + std::to_string(n.z) + std::to_string(uv.x) + std::to_string(uv.y);

		return ret;
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
	MtlData theMtlData;
	std::string map_Kd, map_Ks, map_Ka;
	int one, two, three;
	Material(float Ns, DirectX::XMFLOAT3 kd, DirectX::XMFLOAT3 ks, DirectX::XMFLOAT3 ka, std::string map_Kd, std::string map_Ks, std::string map_Ka)
		:map_Kd(map_Kd), map_Ks(map_Ks), map_Ka(map_Ka)
	{
		if (map_Kd == "Texturen\\ground.png") {
			int dkjsd = 0;
		}
		theMtlData.Ns = Ns;
		theMtlData.kd = kd;
		theMtlData.ks = ks;
		theMtlData.ka = ka;
		one = -1;
		two = -1;
		three = -1;
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
	bool SetupTexture(ID3D11Device* device, MtlImages* mtlFileTex) {

		bool found = false;
		bool found1 = false;
		bool found2 = false;
		//int one, two, three;
		int i = 0;
		int size = (int)mtlFileTex->names.size();
		for (i = 0; i < size; i++) {
			if (mtlFileTex->names[i] == map_Kd) {
				found = true;
				one = i;
			}
			else if (mtlFileTex->names[i] == map_Ks) {
				found1 = true;
				two = i;
			}
			else if (mtlFileTex->names[i] == map_Ka) {
				found2 = true;
				three = i;
			}
			if (found && found1 && found2) {
				break;
			}
		}
		//Setting deafult texture if it was not found (0 == deafult)
		if (!found) {
			one = 0; 
		}
		if (!found1) {
			two = 0;
		}
		if (!found2) {
			three = 0;
		}

		return true;
	}
	bool CreateSRV(ID3D11Device* device, ID3D11ShaderResourceView* srv[], MtlImages* mtlFileTex) {
	
		if (FAILED(device->CreateShaderResourceView(mtlFileTex->textures[one], nullptr, &srv[0]))) {
			return false;
		}
		if (FAILED(device->CreateShaderResourceView(mtlFileTex->textures[two], nullptr, &srv[1]))) {
			return false;
		}
		if (FAILED(device->CreateShaderResourceView(mtlFileTex->textures[three], nullptr, &srv[2]))) {
			return false;
		}
		return true;
	}
	bool SetUpCbuf(ID3D11Device* device, ID3D11Buffer*& cbuf) {
		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = roundUpTo(sizeof(MtlData), 16);
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = &theMtlData;
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
		int size = (int)indices.size();
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

		DirectX::XMFLOAT3 neg = { -1.0f, -1.0f, -1.0f };
		float Ns = 0.0f;
		DirectX::XMFLOAT3 kd = { 0.1f, 0.1f, 0.1f };
		DirectX::XMFLOAT3 ks = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 ka = { 0.1f, 0.1f, 0.1f };
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
		bool foundMapKd = false;
		bool foundMapKa = false;
		bool foundMapKs = false;
		if (fileName == "ground.mtl") {
			int kldsd = 0;
		}
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
				else {
					break;
				}

			}
			if (found) {
				if (prefix == "Ns") {
					ss >> Ns;
				}
				else if (prefix == "Kd") {
					ss >> kd.x >> kd.y >> kd.z;
				}
				else if (prefix == "Ks") {
					ss >> ks.x >> ks.y >> ks.z;
				}
				else if (prefix == "Ka") {
					ss >> ka.x >> ka.y >> ka.z;
				}
				else if (prefix == "map_Kd") {
					ss >> map_Kd;
					foundMapKd = true;
				}
				else if (prefix == "map_Ks") {
					ss >> map_Ks;
					foundMapKs = true;
				}
				else if (prefix == "map_Ka") {
					ss >> map_Ka;
					foundMapKa = true;
				}
			}
		}
		Material mat(Ns, kd, ka, ks, map_Kd, map_Ks, map_Ka);
		if (!mat.SetupTexture(device, mtlFileTex)) {
			return false;
		}
		if (!mat.CreateSRV(device, srv, mtlFileTex)) {
			return false;
		}
		if (!mat.SetUpCbuf(device, cbuf)) {
			return false;
		}

		context->PSSetShaderResources(0, 3, srv);
		context->CSSetConstantBuffers(0, 1, &cbuf);

		return true;
	}

	void Bind(ID3D11DeviceContext*& context, int flag = NORMAL) {
		if (flag == NORMAL || flag == CUBE_MAP || flag == CUBE_MAP_TWO) {
			context->PSSetShaderResources(0, 3, srv);
			context->CSSetConstantBuffers(0, 1, &cbuf);
		}
		context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

		context->DrawIndexed((UINT)subIndices.size(), 0, 0);

	}

	void Terminate() {
		if (indexBuffer)indexBuffer->Release();
		if (cbuf)cbuf->Release();
		for (int i = 0; i < 3; i++) {
			if (srv[i])srv[i]->Release();
		}
	}
};

