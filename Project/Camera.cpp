#include "Camera.h"



/*Camera::Camera()
{
	Reset();
}

Camera::Camera(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 dir)
{
	SetPos(pos);
	SetDir(dir);
}

Camera::~Camera()
{

}

DirectX::XMMATRIX Camera::GetMatrix()
{
	DirectX::XMVECTOR eyePos = DirectX::XMVectorSet(position.x, position.y, position.z, 1.0f);
	DirectX::XMVECTOR lookAtPos = DirectX::XMVectorSet(position.x + direction.x, position.y + direction.y, position.z + direction.z, 1.0f);
	DirectX::XMVECTOR upDir = DirectX::XMVectorSet(upDirection.x, upDirection.y, upDirection.z, 0.0f);

	return  DirectX::XMMatrixLookAtLH(eyePos, lookAtPos, upDir);
}

DirectX::XMFLOAT3* Camera::GetPos()
{
	return &position;
}

DirectX::XMFLOAT3* Camera::GetDir()
{
	return &direction;
}


Frustum* Camera::GetFrustum()
{
	return &this->frustum;
}

float* Camera::GetFrustumBuffer()
{
	return frustum.GetBuffer();
}

DirectX::XMFLOAT3 Camera::GetForward()
{
	return this->direction;
}

DirectX::XMFLOAT3 Camera::GetUp()
{
	DirectX::XMFLOAT3 up;
	up = normalize(cross(GetForward(), GetRight()));
	return up;
}

DirectX::XMFLOAT3 Camera::GetRight()
{
	return normalize(DirectX::XMFLOAT3(direction.z, direction.y, -direction.x));
}

float Camera::GetAngle()
{
	//updateFrustum();
	return this->angle * 57.29578f;
}

void Camera::SetPos(DirectX::XMFLOAT3 pos)
{
	position = pos;
}

void Camera::SetDir(DirectX::XMFLOAT3 dir)
{
	direction = dir;
}

void Camera::SetUpDir(DirectX::XMFLOAT3 dir)
{
	upDirection = dir;
}

void Camera::SetProjection(DirectX::XMMATRIX proj)
{
	//this->frustum.CreateFromMatrix(this->frustum, proj);
}

void Camera::Reset()
{
	position = DirectX::XMFLOAT3(0.0f, 0.0f, -3.0f);
	direction = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
	upDirection = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	speed = 0.1f;
}

void Camera::RotateSide(float rot)
{
	DirectX::XMFLOAT3 tempDir = direction;
	direction.x = tempDir.x * cos(rot) + tempDir.z * sin(rot);
	direction.y = tempDir.y;
	direction.z = -tempDir.x * sin(rot) + tempDir.z * cos(rot);
}

void Camera::RotateVertical(float rot)
{


}

void Camera::forward()
{
	position.x += direction.x * speed;
	position.z += direction.z * speed;
}

void Camera::backward()
{
	//180 deg
	position.x -= direction.x * speed;
	position.z -= direction.z * speed;
}

void Camera::left()
{
	//90 deg counterclockwise
	position.x -= direction.z * speed;
	position.z += direction.x * speed;
}

void Camera::right()
{
	//90 deg clockwise
	position.x += direction.z * speed;
	position.z -= direction.x * speed;
}

void Camera::up()
{
	position.y += speed;
}

void Camera::down()
{
	position.y -= speed;
}

void Camera::Update(float nearZ, float farZ, float width, float height, float fov)
{
	this->upDirection = GetUp();
	this->frustum.SetFrustum(this->position, nearZ, farZ, width, height, fov, this->GetForward(), GetUp(), this->GetRight());
}

bool Camera::Intersect(DirectX::BoundingBox box)
{
	return this->frustum.intersect(box);
}

bool Camera::Intersect(DirectX::BoundingSphere sphere)
{
	return this->frustum.intersect(sphere);
}

float Camera::DegToRad(float deg)
{
	double pi = 3.14159265359;
	return (deg * (pi / 180));
}
*/

Camera::Camera()
{
	this->pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->posVector = XMLoadFloat3(&this->pos);
	this->rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->speed = 1.0f;
	this->rotSpeed = 2.0f;
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

void Camera::SetPosition(const XMVECTOR& pos)
{
	XMStoreFloat3(&this->pos, pos);
	this->posVector = pos;
	this->UpdateViewMatrix();
}

void Camera::SetPosition(const XMFLOAT3& pos)
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

void Camera::SetRotation(const XMVECTOR& rot)
{
	XMStoreFloat3(&this->rot, rot);
	this->posVector = rot;
	this->UpdateViewMatrix();
}

void Camera::SetRotation(const XMFLOAT3& rot)
{
	this->rot = rot;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
}

void Camera::SetRotation(float x, float y, float z)
{
	this->rot.x = x;
	this->rot.y = y;
	this->rot.z = z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
}

void Camera::Rotate(const XMVECTOR& rot)
{
	this->rotVector += rot * rotSpeed;
	XMStoreFloat3(&this->rot, this->rotVector);
	this->UpdateViewMatrix();
}

void Camera::Rotate(const XMFLOAT3& rot)
{
	this->rot.x += rot.x * rotSpeed;
	this->rot.y += rot.y * rotSpeed;
	this->rot.z += rot.z * rotSpeed;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
}

void Camera::Rotate(float x, float y, float z)
{
	this->rot.x += x * rotSpeed;
	this->rot.y += y * rotSpeed;
	this->rot.z += z * rotSpeed;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
}

bool Camera::Intersect(DirectX::BoundingBox box)
{
	return this->frustum.intersect(box);
}

bool Camera::Intersect(DirectX::BoundingSphere sphere)
{
	return this->frustum.intersect(sphere);
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

	UpdateFrustum();
}

void Camera::UpdateFrustum()
{
	XMFLOAT3 forwardFloat3, upFloat3, rightFloat3;
	XMStoreFloat3(&forwardFloat3, forward);
	XMStoreFloat3(&upFloat3, up);
	XMStoreFloat3(&rightFloat3, right);
	this->frustum.SetFrustum(this->pos, nearZ, farZ, width, height, fov, forwardFloat3, upFloat3, rightFloat3);
}
