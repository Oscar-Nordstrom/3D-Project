#include "SceneObjectTest.h"
#include "Graphics.h"

namespace dx = DirectX;

SceneObjectTest::SceneObjectTest()
{

	transform = dx::XMMatrixScaling(sx, sy, sz) * dx::XMMatrixRotationX(rx) * dx::XMMatrixRotationY(ry) * dx::XMMatrixRotationZ(rz) * dx::XMMatrixTranslation(x, y, z);
	boundingSphere.Center = DirectX::XMFLOAT3(x, y, z);
	boundingSphere.Radius = LargestSide();// / 2.0f;

	this->added = false;
}

SceneObjectTest::~SceneObjectTest()
{
}



bool SceneObjectTest::Update(float dt, Graphics*& gfx)
{
	UpdateTransform(dt);
	if (!model.UpdateCbuf(*gfx, transform, this->x, this->y, this->z)) {
		assert(false && "Failed to update constant buffer.");
		std::cerr << "Failed to update constant buffer.\n";
		return false;
	}
	return true;
}

void SceneObjectTest::UpdateParticle(Graphics*& gfx)
{
	model.SetParticleUpdate(gfx);
}

void SceneObjectTest::Move(float dx, float dy, float dz)
{
	x += dx;
	y += dy;
	z += dz;
	boundingSphere.Center = DirectX::XMFLOAT3(x, y, z);
}

void SceneObjectTest::SetPos(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
	boundingSphere.Center = DirectX::XMFLOAT3(this->x, this->y, this->z);
}

void SceneObjectTest::SetPos(DirectX::XMFLOAT3 pos)
{
	this->x = pos.x;
	this->y = pos.y;
	this->z = pos.z;
	boundingSphere.Center = DirectX::XMFLOAT3(this->x, this->y, this->z);
}

void SceneObjectTest::Scale(float dx, float dy, float dz)
{

	sx += dx;
	sy += dy;
	sz += dz;
	boundingSphere.Radius = LargestSide() / 2.0f;
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

DirectX::BoundingSphere SceneObjectTest::GetBoundingSphere()
{
	return boundingSphere;
}

void SceneObjectTest::Add()
{
	this->added = true;
}

void SceneObjectTest::Remove()
{
	this->added = false;
}

bool SceneObjectTest::IsAdded() const
{
	return this->added;
}

float SceneObjectTest::GetX() const
{
	return this->x;
}

float SceneObjectTest::GetY() const
{
	return this->y;
}

float SceneObjectTest::GetZ() const
{
	return this->z;
}

bool SceneObjectTest::Init(TextureHandler*& texHandl, string objPath, string vShaderPath, string hShaderPath, string dShaderPath, string pShaderPath, string cShaderPath, string gShaderPath, Graphics*& gfx, bool particle)
{
	this->model.SetTexHandl(texHandl);

	if (!particle) {
		assert(model.Load(objPath, vShaderPath, hShaderPath, dShaderPath, pShaderPath, cShaderPath, gShaderPath, transform, gfx) && "Failed to load model.");
	}
	else {
		assert(model.LoadAsParticle(vShaderPath, gShaderPath, pShaderPath, cShaderPath, transform, gfx) && "Failed to load model.");
	}

	return true;
}

void SceneObjectTest::UpdateTransform(float dt)
{
	Rotate(0.0f, dt, 0.0f);
	transform = dx::XMMatrixScaling(sx, sy, sz) * dx::XMMatrixRotationX(rx) * dx::XMMatrixRotationY(ry) * dx::XMMatrixRotationZ(rz) * dx::XMMatrixTranslation(x, y, z);
}

float SceneObjectTest::LargestSide() const
{
	if (sx > sy && sx > sz) {
		return sx;
	}
	else if (sy > sx && sy > sz) {
		return sy;
	}
	else {
		return sz;
	}
}
