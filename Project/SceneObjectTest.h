#pragma once
#include "Graphics.h"
#include "Model.h"
#include <DirectXMath.h>
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
	SceneObjectTest(Graphics& gfx);
	~SceneObjectTest();

	bool Init(string objPath, string vShaderPath, string hShaderPath, string dShaderPath, string pShaderPath, string cShaderPath, Graphics*& gfx);
	bool Update(float dt, Graphics*& gfx);
	void Move(float dx, float dy, float dz);
	void Scale(float dx, float dy, float dz);
	void Rotate(float dx, float dy, float dz);
	void Draw(Graphics*& gfx, int flag = NORMAL);
private:

private:
	void UpdateTransform(float dt);
private:
	Model model;
	DirectX::XMMATRIX transform;

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