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

	bool Load(string obj, string vShader, string pShader, string cShader, Graphics*& gfx);
private:
	bool LoadShaders(string vShader, string pShader, string cShader);
	bool LoadObj(string obj);
private:
	vector<DirectX::XMFLOAT3> v;
	vector<DirectX::XMFLOAT3> vn;
	vector<DirectX::XMFLOAT2> vt;

	vector<SimpleVertex> verts; 
	vector<unsigned short> indices;

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