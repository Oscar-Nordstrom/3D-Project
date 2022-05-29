#include "Camera.h"


Camera::Camera()
{
	this->pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->posVector = XMLoadFloat3(&this->pos);
	this->rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->speed = 1.0f;
	this->rotSpeed = 1.0f;
	this->UpdateViewMatrix();
	this->UpdateFrustum();
}

Camera::~Camera()
{
}

void Camera::SetProj(float fov, int width, int height, float nearZ, float farZ)
{
	float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	float fovRadius = (fov / 360.0f) * XM_2PI;
	this->projectionMatrix = XMMatrixPerspectiveFovLH(fovRadius, aspectRatio, nearZ, farZ);
	this->width = width;
	this->height = height;
	this->fov = fov;
	this->nearZ = nearZ;
	this->farZ = farZ;
}

const XMMATRIX& Camera::GettViewMatrix() const
{
	return this->viewMatrix;
}

const XMMATRIX& Camera::GettProjectionMatrix() const
{
	return this->projectionMatrix;
}

const XMVECTOR& Camera::GetPositionVector() const
{
	return this->posVector;
}

const XMFLOAT3& Camera::GetPositionFloat3() const
{
	return this->pos;
}

const XMVECTOR& Camera::GetRotationVector() const
{
	return this->rotVector;
}

const XMFLOAT3& Camera::GetRotationFloat3() const
{
	return this->rot;
}

const XMVECTOR& Camera::GetForwardVec()
{
	return this->forward;
}

const XMVECTOR& Camera::GetBackVec()
{
	return this->back;
}

const XMVECTOR& Camera::GetUpVec()
{
	return this->up;
}

const XMVECTOR& Camera::GetDownVec()
{
	return this->down;
}

const XMVECTOR& Camera::GetLeftVec()
{
	return this->left;
}

const XMVECTOR& Camera::GetRightVec()
{
	return this->right;
}

const XMFLOAT3& Camera::GetForwardFloat3()
{
	return this->forwardFloat3;
}

const XMFLOAT3& Camera::GetBackFloat3()
{
	return this->backFloat3;
}

const XMFLOAT3& Camera::GetUpFloat3()
{
	return this->upFloat3;
}

const XMFLOAT3& Camera::GetDownFloat3()
{
	return this->downFloat3;
}

const XMFLOAT3& Camera::GetLeftFloat3()
{
	return this->leftFloat3;
}

const XMFLOAT3& Camera::GetRightFloat3()
{
	return this->rightFloat3;
}

float& Camera::GetSpeed()
{
	return this->speed;
}

float& Camera::GetRotSpeed()
{
	return this->rotSpeed;
}

float* Camera::GetFrustumBuffer()
{
	return frustum.GetBuffer();
}

Frustum* Camera::GetFrustum()
{
	return &this->frustum;
}

void Camera::SetPosition( XMVECTOR& pos)
{
	XMStoreFloat3(&this->pos, pos);
	this->posVector = pos;
	this->UpdateViewMatrix();
}

void Camera::SetPosition( XMFLOAT3& pos)
{
	this->pos = pos;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateViewMatrix();
}

void Camera::SetPosition(float x, float y, float z)
{
	this->pos.x = x;
	this->pos.y = y;
	this->pos.z = z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateViewMatrix();
}

void Camera::Move(const XMVECTOR& pos)
{
	this->posVector += pos * this->speed;
	XMStoreFloat3(&this->pos, this->posVector);
	this->UpdateViewMatrix();
}

void Camera::Move(const XMFLOAT3& pos)
{
	this->pos.x += pos.x * this->speed;
	this->pos.y += pos.y * this->speed;
	this->pos.z += pos.z * this->speed;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateViewMatrix();
}

void Camera::Move(float x, float y, float z)
{
	this->pos.x += x * this->speed;
	this->pos.y += y * this->speed;
	this->pos.z += z * this->speed;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateViewMatrix();
}

void Camera::SetRotationRad(const XMVECTOR& rot)
{
	XMStoreFloat3(&this->rot, rot);
	this->rotVector = rot;
	this->UpdateViewMatrix();
}

void Camera::SetRotationRad(const XMFLOAT3& rot)
{
	this->rot = rot;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
}

void Camera::SetRotationRad(float x, float y, float z)
{
	this->rot.x = x;
	this->rot.y = y;
	this->rot.z = z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
}

void Camera::RotateRad( XMVECTOR& rot)
{
	this->CheckRotation(rot);
	this->rotVector += rot * rotSpeed;
	XMStoreFloat3(&this->rot, this->rotVector);
	this->UpdateViewMatrix();
}

void Camera::RotateRad( XMFLOAT3& rot)
{
	this->CheckRotation(rot);
	this->rot.x += rot.x * rotSpeed;
	this->rot.y += rot.y * rotSpeed;
	this->rot.z += rot.z * rotSpeed;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
}

void Camera::RotateRad(float x, float y, float z)
{
	this->CheckRotation(x,y,z);
	this->rot.x += x * rotSpeed;
	this->rot.y += y * rotSpeed;
	this->rot.z += z * rotSpeed;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();

}

void Camera::SetRotationDeg(const XMVECTOR& rot)
{
	XMFLOAT3 rotation;
	XMStoreFloat3(&rotation, rot);
	rotation.x = XMConvertToRadians(rotation.x);
	rotation.y = XMConvertToRadians(rotation.y);
	rotation.z = XMConvertToRadians(rotation.z);
	
	this->rot = rotation;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
}

void Camera::SetRotationDeg(const XMFLOAT3& rot)
{
	XMFLOAT3 rotation = rot;
	rotation.x = XMConvertToRadians(rotation.x);
	rotation.y = XMConvertToRadians(rotation.y);
	rotation.z = XMConvertToRadians(rotation.z);

	this->rot = rotation;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
}

void Camera::SetRotationDeg(float x, float y, float z)
{
	this->rot.x = XMConvertToRadians(x);
	this->rot.y = XMConvertToRadians(y);
	this->rot.z = XMConvertToRadians(z);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
}

void Camera::RotateDeg( XMVECTOR& rot)
{
	this->CheckRotationDeg(rot);
	XMFLOAT3 rotation;
	XMStoreFloat3(&rotation, rot);
	rotation.x =+ XMConvertToRadians(rotation.x) * rotSpeed;
	rotation.y =+ XMConvertToRadians(rotation.y) * rotSpeed;
	rotation.z =+ XMConvertToRadians(rotation.z) * rotSpeed;

	this->rot = rotation;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
}

void Camera::RotateDeg( XMFLOAT3& rot)
{
	this->CheckRotationDeg(rot);
	XMFLOAT3 rotation = rot;
	rotation.x =+ XMConvertToRadians(rotation.x) * rotSpeed;
	rotation.y =+ XMConvertToRadians(rotation.y) * rotSpeed;
	rotation.z =+ XMConvertToRadians(rotation.z) * rotSpeed;

	this->rot = rotation;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
}

void Camera::RotateDeg(float x, float y, float z)
{
	this->CheckRotationDeg(x,y,z);
	this->rot.x =+ XMConvertToRadians(x) * rotSpeed;
	this->rot.y =+ XMConvertToRadians(y) * rotSpeed;
	this->rot.z =+ XMConvertToRadians(z) * rotSpeed;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
}

bool Camera::Intersect(DirectX::BoundingBox box)
{
	return this->frustum.intersect(box);
	//return this->testFrustum.Intersects(box);
}

bool Camera::Intersect(DirectX::BoundingSphere sphere)
{
	return this->frustum.intersect(sphere);
	//return this->testFrustum.Intersects(sphere);
}

void Camera::UpdateViewMatrix()
{
	XMMATRIX camRotMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z);
	XMVECTOR camTarget = XMVector3TransformCoord(this->DefaultForwardVector, camRotMatrix);
	camTarget += this->posVector;
	XMVECTOR upDir = XMVector3TransformCoord(this->DefaultUpVector, camRotMatrix);
	this->viewMatrix = XMMatrixLookAtLH(this->posVector, camTarget, upDir);

	//Updating directions
	XMMATRIX vecRotMatrix = XMMatrixRotationRollPitchYaw(0.0f, this->rot.y, 0.0f);
	this->forward = XMVector3TransformCoord(this->DefaultForwardVector, vecRotMatrix);
	this->back = XMVector3TransformCoord(this->DefaultBackVector, vecRotMatrix);
	this->left = XMVector3TransformCoord(this->DefaultLeftVector, vecRotMatrix);
	this->right = XMVector3TransformCoord(this->DefaultRightVector, vecRotMatrix);
	this->up = upDir;
	this->down = -upDir;
	XMStoreFloat3(&this->forwardFloat3, this->forward);
	XMStoreFloat3(&this->backFloat3, this->back);
	XMStoreFloat3(&this->leftFloat3, this->left);
	XMStoreFloat3(&this->rightFloat3, this->right);
	XMStoreFloat3(&this->upFloat3, this->up);
	XMStoreFloat3(&this->downFloat3, this->down);

	UpdateFrustum();
}

void Camera::UpdateFrustum()
{
	this->frustum.SetFrustum(this->pos, this->nearZ, this->farZ, this->width, this->height, this->fov, this->forwardFloat3, this->upFloat3, this->rightFloat3);
	//DirectX::BoundingFrustum::CreateFromMatrix(this->testFrustum, this->projectionMatrix);
}

void Camera::CheckRotation(XMVECTOR& rot)
{
	DirectX::XMFLOAT3 temp;
	DirectX::XMStoreFloat3(&temp, rot);
	if (abs(DirectX::XMConvertToDegrees(this->rot.x + temp.x)) > 90.0f) {
		temp.x = 0.0f;
	}
	if (abs(DirectX::XMConvertToDegrees(this->rot.z + temp.z)) > 90.0f) {
		temp.z = 0.0f;
	}
	rot = DirectX::XMLoadFloat3(&temp);
}

void Camera::CheckRotation(XMFLOAT3& rot)
{
	if (abs(DirectX::XMConvertToDegrees(this->rot.x + rot.x)) > 90.0f) {
		rot.x = 0.0f;
	}
	if (abs(DirectX::XMConvertToDegrees(this->rot.z + rot.z)) > 90.0f) {
		rot.z = 0.0f;
	}
}

void Camera::CheckRotation(float& x, float& y, float& z)
{
	if (abs(DirectX::XMConvertToDegrees(this->rot.x + x)) > 90.0f) {
		x = 0.0f;
	}
	if (abs(DirectX::XMConvertToDegrees(this->rot.z + z)) > 90.0f) {
		z = 0.0f;
	}
}

void Camera::CheckRotationDeg(XMVECTOR& rot)
{
	DirectX::XMFLOAT3 temp;
	DirectX::XMStoreFloat3(&temp, rot);
	if (abs(this->rot.x + temp.x) > 90.0f) {
		temp.x = 0.0f;
	}
	if (abs(this->rot.z + temp.z) > 90.0f) {
		temp.z = 0.0f;
	}
	rot = DirectX::XMLoadFloat3(&temp);
}

void Camera::CheckRotationDeg(XMFLOAT3& rot)
{
	if (abs(this->rot.x + rot.x) > 90.0f) {
		rot.x = 0.0f;
	}
	if (abs(this->rot.z + rot.z) > 90.0f) {
		rot.z = 0.0f;
	}
}

void Camera::CheckRotationDeg(float& x, float& y, float& z)
{
	if (abs(this->rot.x + x) > 90.0f) {
		x = 0.0f;
	}
	if (abs(this->rot.z + z) > 90.0f) {
		z = 0.0f;
	}
}
