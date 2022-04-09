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
	DirectX::XMMATRIX vecRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(0.0f, direction.y, 0.0f);
	left = DirectX::XMVector3TransformCoord(DefaultLeft, vecRotationMatrix);
	right = DirectX::XMVector3TransformCoord(DefaultRight, vecRotationMatrix);
	forward = DirectX::XMVector3TransformCoord(DefaultForward, vecRotationMatrix);
	backward = DirectX::XMVector3TransformCoord(DefaultBackward, vecRotationMatrix);

	DirectX::XMVECTOR eyePos = DirectX::XMVectorSet(position.x, position.y, position.z, 1.0f);//Position of the camera
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

void Camera::Move(DirectX::XMVECTOR vec)
{
	DirectX::XMFLOAT3 temp;
	DirectX::XMStoreFloat3(&temp, vec);
	position.x += temp.x;
	position.y += temp.y;
	position.z += temp.z;
}

DirectX::XMVECTOR Camera::GetLeftVector()
{
	return left;
}

DirectX::XMVECTOR Camera::GetRightVector()
{
	return right;
}

DirectX::XMVECTOR Camera::GetForwardVector()
{
	return forward;
}

DirectX::XMVECTOR Camera::GetBackwardVector()
{
	return backward;
}
