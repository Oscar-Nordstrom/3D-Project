#include "Camera.h"

Camera::Camera()
{
	Reset();
}

Camera::~Camera()
{
}

DirectX::XMMATRIX Camera::GetMatrix() const
{
	DirectX::XMVECTOR eyePos = DirectX::XMVectorSet(position.x, position.y, position.z, 0.0f);//Position of the camera
	DirectX::XMVECTOR lookAtPos = DirectX::XMVectorSet(direction.x, direction.y, direction.z, 0.0f);//Direction the camera is looking at
	DirectX::XMVECTOR upDir = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);//The up direction

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

void Camera::SetPos(DirectX::XMFLOAT3 pos)
{
	if (pos.x == 0.0f) {
		pos.x = 0.01f;
	}
	if (pos.y == 0.0f) {
		pos.y = 0.01f;
	}
	if (pos.z == 0.0f) {
		pos.z = 0.01f;
	}
	position = pos;
}

void Camera::SetDir(DirectX::XMFLOAT3 dir)
{
	direction = dir;
}

void Camera::Reset()
{
	position = DirectX::XMFLOAT3(0.0f, 0.0f, -3.0f);
	direction = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
}
