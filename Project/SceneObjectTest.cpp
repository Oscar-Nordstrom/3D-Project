#include "SceneObjectTest.h"
#include "Graphics.h"

namespace dx = DirectX;

SceneObjectTest::SceneObjectTest(Graphics& gfx)
{

	transform = dx::XMMatrixScaling(sx, sy, sz) * dx::XMMatrixRotationX(rx) * dx::XMMatrixRotationY(ry) * dx::XMMatrixRotationZ(rz) * dx::XMMatrixTranslation(x, y, z);
}

SceneObjectTest::~SceneObjectTest()
{

}



bool SceneObjectTest::Update(float dt, Graphics*& gfx)
{
	UpdateTransform(dt);
	if (!model.UpdateCbuf(*gfx, transform)) {
		assert(false, "Failed to update constant buffer.");
		std::cerr << "Failed to update constant buffer.\n";
		return false;
	}
	return true;
}

void SceneObjectTest::Move(float dx, float dy, float dz)
{
	x += dx;
	y += dy;
	z += dz;
}

void SceneObjectTest::Scale(float dx, float dy, float dz)
{

	sx += dx;
	sy += dy;
	sz += dz;
}

void SceneObjectTest::Rotate(float dx, float dy, float dz)
{
	rx += dx;
	ry += dy;
	rz += dz;
}

void SceneObjectTest::Draw(Graphics*& gfx, int flag)
{
	model.Draw(gfx, transform, flag);

}

void SceneObjectTest::DisableTesselation()
{
	model.DisableTesselation();
}

void SceneObjectTest::EnableTesselation()
{
	model.EnableTesselation();
}

bool SceneObjectTest::Init(string objPath, string vShaderPath, string hShaderPath, string dShaderPath, string pShaderPath, string cShaderPath, string gShaderPath, Graphics*& gfx, bool particle)
{
	if (!particle) {
		assert(model.Load(objPath, vShaderPath, hShaderPath, dShaderPath, pShaderPath, cShaderPath, gShaderPath, transform, gfx), "Failed to load model.");
	}
	else {
		assert(model.LoadAsParticle(vShaderPath, gShaderPath, pShaderPath, transform, gfx), "Failed to load model.");
	}

	return true;
}

void SceneObjectTest::UpdateTransform(float dt)
{
	Rotate(0.0f, dt, 0.0f);
	transform = dx::XMMatrixScaling(sx, sy, sz) * dx::XMMatrixRotationX(rx) * dx::XMMatrixRotationY(ry) * dx::XMMatrixRotationZ(rz) * dx::XMMatrixTranslation(x, y, z);
}