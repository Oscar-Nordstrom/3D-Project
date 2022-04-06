#pragma once
#include "Graphics.h"
#include "Structures.h"
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <Windows.h>


using namespace std;

class SceneObject {
public:
	SceneObject(Graphics& gfx);
	~SceneObject();

	bool Init(string meshPath, string vShaderPath, string pShaderPath, string cShaderPath, Graphics& gfx);
	bool Update(float dt, Graphics& gfx);
	void Move(float dx, float dy, float dz);
	void Scale(float dx, float dy, float dz);
	void Rotate(float dx, float dy, float dz);
	void Draw(Graphics*& gfx, bool withShaders = true);
private:
	bool LoadShaders(string vShaderPath, string pShaderPath, string cShaderPath, ID3D11Device* const& device);
	bool LoadMesh(ID3D11Device* device, ID3D11DeviceContext* context, string meshPath);
	bool CreateInputLayout(ID3D11Device* device);
	bool SetUpSampler(ID3D11Device* device);
	bool CreateVertexBuffer(ID3D11Device* device);
	bool CreateIndexBuffer(ID3D11Device* device);
	bool CreateConstantBuffer(Graphics& gfx);

	bool UpdateCbuf(Graphics& gfx);

private:
	int GetIndex(SimpleVertex v);
	void UpdateTransform(float dt);
private:

	DirectX::XMMATRIX transform;

	vector<DirectX::XMFLOAT3> v;
	vector<DirectX::XMFLOAT3> vn;
	vector<DirectX::XMFLOAT2> vt;

	vector<SimpleVertex> verts;
	vector<unsigned short> indices;
	vector<SubMesh> subs;

	MtlImages* mtlFileTex;


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

private:
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float sx = 1.0f;
	float sy = 1.0f;
	float sz = 1.0f;
	float rx = 0.0f;
	float ry = 0.0f;
	float rz = 0.0f;
};