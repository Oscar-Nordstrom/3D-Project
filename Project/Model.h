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

#define VERTEX_SHADER 0
#define HULL_SHADER 1
#define DOMAIN_SHADER 2
#define PIXEL_SHADER 3
#define COMPUTE_SHADER 4


using namespace std;

class Model {
public:
	Model();
	~Model();

	bool Load(string obj, string vShaderPath, string hShaderPath, string dShaderPath, string pShaderPath, string cShaderPath, DirectX::XMMATRIX transform, Graphics*& gfx);
	void Draw(Graphics*& gfx, DirectX::XMMATRIX transform, bool withShaders = true);
	bool UpdateCbuf(Graphics& gfx, DirectX::XMMATRIX transform);
private:
	bool LoadShaders(string vShaderPath, string hShaderPath, string dShaderPath, string pShaderPath, string cShaderPath, Graphics*& gfx);
	bool LoadObj(string obj, Graphics*& gfx);
	bool CreateInputLayout(ID3D11Device*& device);
	bool SetUpSampler(ID3D11Device*& device);
	bool CreateVertexBuffer(ID3D11Device*& device);
	bool CreateIndexBuffer(ID3D11Device*& device);
	bool CreateConstantBuffer(Graphics& gfx, DirectX::XMMATRIX transform);

	bool ReadShader(Graphics*& gfx, string path, int flag, ID3D11VertexShader*& v, ID3D11HullShader*& h, ID3D11DomainShader*& d, ID3D11PixelShader*& p, ID3D11ComputeShader*& c);

	
private:
	vector<DirectX::XMFLOAT3> v;
	vector<DirectX::XMFLOAT3> vn;
	vector<DirectX::XMFLOAT2> vt;

	vector<SimpleVertex> verts; 
	vector<unsigned short> indices;
	vector<SubMesh*> subs;


	MtlImages* images;
	

	string vShaderByteCode;
	ID3D11VertexShader* vShader;
	ID3D11HullShader* hShader;
	ID3D11DomainShader* dShader;
	ID3D11PixelShader* pShader;
	ID3D11ComputeShader* cShader;
	ID3D11InputLayout* inputLayout;
	ID3D11SamplerState* samState;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* constantBuffer;
	D3D11_PRIMITIVE_TOPOLOGY topology;
	D3D11_PRIMITIVE_TOPOLOGY topologyTriList;

	ifstream file;
};