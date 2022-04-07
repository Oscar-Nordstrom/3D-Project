#pragma once
#include <DirectXMath.h>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <map>
#include <cassert>

#include "Structures.h"
#include "Graphics.h"


using namespace std;

class Model {
public:
	Model();
	~Model();

	bool Load(string obj, string vShader, string pShader, string cShader, DirectX::XMMATRIX transform, Graphics*& gfx);
	void Draw(Graphics*& gfx, DirectX::XMMATRIX transform, bool withShaders = true);
	bool UpdateCbuf(Graphics& gfx, DirectX::XMMATRIX transform);
private:
	bool LoadShaders(string vShaderPath, string pShaderPath, string cShaderPath, Graphics*& gfx);
	bool LoadObj(string obj, Graphics*& gfx);
	bool CreateInputLayout(ID3D11Device*& device);
	bool SetUpSampler(ID3D11Device*& device);
	bool CreateVertexBuffer(ID3D11Device*& device);
	bool CreateIndexBuffer(ID3D11Device*& device);
	bool CreateConstantBuffer(Graphics& gfx, DirectX::XMMATRIX transform);

	
private:
	vector<DirectX::XMFLOAT3> v;
	vector<DirectX::XMFLOAT3> vn;
	vector<DirectX::XMFLOAT2> vt;

	vector<SimpleVertex> verts; 
	vector<unsigned short> indices;
	vector<SubMesh> subs;


	MtlImages* images;
	

	string vShaderByteCode;
	ID3D11VertexShader* vShader;
	ID3D11PixelShader* pShader;
	ID3D11ComputeShader* cShader;
	ID3D11InputLayout* inputLayout;
	ID3D11SamplerState* samState;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* constantBuffer;
	D3D11_PRIMITIVE_TOPOLOGY topology;

	ifstream file;
};