#include "Camera.h"

Camera::Camera()
{
	Reset();
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

/*DirectX::BoundingFrustum Camera::GetFrustum()
{
	updateFrustum();
	return this->frustum;
}*/

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
	return DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
}

DirectX::XMFLOAT3 Camera::GetRight()
{
	return DirectX::XMFLOAT3(direction.z, direction.y, -direction.x);
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

void Camera::Rotate(float rot)
{
	rot = -rot;
	DirectX::XMFLOAT3 tempDir = direction;

	direction.x = tempDir.x * cos(rot) - tempDir.z * sin(rot);
	direction.z = tempDir.x * sin(rot) + tempDir.z * cos(rot);
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
	this->frustum.SetFrustum(this->position, nearZ, farZ, width, height, fov, this->GetForward(), this->GetUp(), this->GetRight());
}

bool Camera::Intersect(DirectX::BoundingBox box)
{
	return this->frustum.intersect(box);
}

bool Camera::Intersect(DirectX::BoundingSphere sphere)
{
	return this->frustum.intersect(sphere);
}

/*void Camera::updateFrustum()
{
	this->frustum.Origin = this->position;
	DirectX::XMFLOAT3 axisFloat = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	DirectX::XMVECTOR axis = DirectX::XMLoadFloat3(&axisFloat);
	DirectX::XMVECTOR newRot = DirectX::XMLoadFloat3(&this->direction);
	DirectX::XMFLOAT3 right = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
	angle = acos(this->direction.x * right.x + this->direction.y * right.y + this->direction.z * right.z);
	if (direction.z < 0) {
		angle = (DegToRad(360)-angle);
	}
	newRot = DirectX::XMQuaternionRotationNormal(axis, angle);
	if (DirectX::Internal::XMQuaternionIsUnit(newRot)) {
		DirectX::XMFLOAT4 orientation;
		DirectX::XMStoreFloat4(&orientation, newRot);
		this->frustum.Orientation = orientation;
	}


}*/

float Camera::DegToRad(float deg)
{
	double pi = 3.14159265359;
	return (deg * (pi / 180));
}




