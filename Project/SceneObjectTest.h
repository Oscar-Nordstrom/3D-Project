#pragma once
#include "Graphics.h"
#include "Model.h"
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <Windows.h>


#include "flags.h"

using namespace std;

class SceneObjectTest {
public:
	SceneObjectTest(/*Graphics& gfx, TextureHandler*& texHandl*/);
	~SceneObjectTest();

	bool Init(TextureHandler*& texHandl, string objPath, string vShaderPath, string hShaderPath, string dShaderPath, string pShaderPath, string cShaderPath, string gShaderPath, Graphics*& gfx, bool particle = false);
	bool Update(float dt, Graphics*& gfx);
	void UpdateParticle(Graphics*& gfx);
	void Move(float dx, float dy, float dz);
	void SetPos(float x, float y, float z);
	void SetPos(DirectX::XMFLOAT3 pos);
	void Scale(float dx, float dy, float dz);
	void Rotate(float dx, float dy, float dz);
	void Draw(Graphics*& gfx, int flag = NORMAL);
	void DisableTesselation();
	void EnableTesselation();
	DirectX::BoundingSphere GetBoundingSphere();
	void Add();
	void Remove();
	bool IsAdded()const;
private:
	void UpdateTransform(float dt);
	float LargestSide()const;
private:
	Model model;
	DirectX::XMMATRIX transform;
	DirectX::BoundingSphere boundingSphere;
	bool added;

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