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




