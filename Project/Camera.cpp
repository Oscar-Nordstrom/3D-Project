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
	DirectX::XMVECTOR eyePos = DirectX::XMVectorSet(position.x, position.y, position.z, 1.0f);//Position of the camera  ////MAY NOT BE ZERO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	DirectX::XMVECTOR lookAtPos = DirectX::XMVectorSet(direction.x, direction.y, direction.z, 1.0f);//Direction the camera is looking at
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

void Camera::Move(DirectX::XMFLOAT3 vec)
{

	position.x += vec.x;
	position.y += vec.y;
	position.z += vec.z;
}



