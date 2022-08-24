#pragma once
#include <DirectXMath.h>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <map>
#include <cassert>

#include <iterator>

#include "Structures.h"
#include "Graphics.h"

#include "flags.h"

#include "FileLoader.h"


using namespace std;

class Model {
public:
	Model();
	~Model();

	bool Load(string obj, string vShaderPath, string hShaderPath, string dShaderPath, string pShaderPath, string cShaderPath, string gShaderPath, DirectX::XMMATRIX transform, Graphics*& gfx);
	bool LoadAsParticle(string vShaderPath, string gShaderPath, string pShaderPath, string cShaderPath, DirectX::XMMATRIX transform, Graphics*& gfx);
	void Draw(Graphics*& gfx, DirectX::XMMATRIX transform, int flag = NORMAL);
	bool UpdateCbuf(Graphics& gfx, DirectX::XMMATRIX transform, float x = 0.0f, float y = 0.0f, float z = 0.0f);
	void DisableTesselation();
	void EnableTesselation();
	void SetParticleUpdate(Graphics*& gfx);
	void SetTexHandl(TextureHandler*& texHandl);
private:
	bool LoadShaders(string vShaderPath, string hShaderPath, string dShaderPath, string pShaderPath, string cShaderPath, string gShaderPath, Graphics*& gfx);
	bool LoadObj(string obj, Graphics*& gfx);
	bool CreateInputLayout(ID3D11Device*& device, bool particle = false);
	bool SetUpSampler(ID3D11Device*& device);
	bool CreateVertexBuffer(ID3D11Device*& device, bool particle = false);
	bool CreateIndexBuffer(ID3D11Device*& device);
	bool CreateConstantBuffer(Graphics& gfx, DirectX::XMMATRIX transform);

	bool ReadShader(Graphics*& gfx, string path, int flag, ID3D11VertexShader*& v, ID3D11HullShader*& h, ID3D11DomainShader*& d, ID3D11PixelShader*& p, ID3D11ComputeShader*& c, ID3D11GeometryShader*& g);
	
private:
	//vector<DirectX::XMFLOAT3> v;
	//vector<DirectX::XMFLOAT3> vn;
	//vector<DirectX::XMFLOAT2> vt;

	vector<SimpleVertex> verts; 
	vector<unsigned short> indices;
	vector<SubMesh*> subs;

	TextureHandler* texHandl;

	string vShaderByteCode;
	ID3D11ShaderResourceView* paprticleTexSrv;
	ID3D11VertexShader* vShader;
	ID3D11HullShader* hShader;
	ID3D11DomainShader* dShader;
	ID3D11PixelShader* pShader;
	ID3D11ComputeShader* cShader;
	ID3D11GeometryShader* gShader;
	ID3D11InputLayout* inputLayout;
	ID3D11SamplerState* samState;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* nullBuf;
	ID3D11UnorderedAccessView* uavBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* constantBuffer;
	ID3D11Buffer* posBuffer;
	ID3D11Buffer* constantBufferTessBool;
	D3D11_PRIMITIVE_TOPOLOGY topology;
	D3D11_PRIMITIVE_TOPOLOGY topologyTriList;
	D3D11_PRIMITIVE_TOPOLOGY topologyPoints;

	FileLoader fileLoader;

	bool tesselation;

	ifstream file;
};