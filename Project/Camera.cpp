#include "Camera.h"


//Old
/*Camera::Camera()
{
	speed = 1.0f;
	Reset();
}

Camera::~Camera()
{
}

DirectX::XMMATRIX Camera::GetMatrix()
{
	DirectX::XMVECTOR eyePos = DirectX::XMVectorSet(position.x, position.y, position.z, 1.0f);//Position of the camera
	DirectX::XMVECTOR lookAtPos = DirectX::XMVectorSet(position.x+direction.x, position.y+direction.y, position.y+direction.z, 1.0f);//Direction the camera is looking at
	//DirectX::XMVECTOR lookAtPos = DirectX::XMVectorSet(position.x, position.y, position.z+1, 1.0f);//Direction the camera is looking at
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

void Camera::Forward()
{
	position.z += speed;
}

void Camera::Backward()
{
	position.z -= speed;
}

void Camera::Left()
{
	position.x -= speed;
}

void Camera::Right()
{
	position.x += speed;
}

void Camera::Up()
{
	position.y += speed;
}

void Camera::Down()
{
	position.y -= speed;
}*/

Camera::Camera()
{
	position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	positionVector = DirectX::XMLoadFloat3(&position);
	rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotationVector = DirectX::XMLoadFloat3(&rotation);
	this->UpdateViewMatrix();
}

Camera::~Camera()
{
}

const DirectX::XMMATRIX& Camera::GetMatrix() const
{
	return viewMatrix;
}

const DirectX::XMVECTOR& Camera::GetPositionVector() 
{
	return positionVector;
}

const DirectX::XMFLOAT3& Camera::GetPositionFloat3() 
{
	return position;
}

const DirectX::XMVECTOR& Camera::GetRotationVector() 
{
	return rotationVector;
}

const DirectX::XMFLOAT3& Camera::GetRotationFloat3() 
{
	return rotation;
}

void Camera::SetPosition( DirectX::XMVECTOR& pos)
{
	DirectX::XMStoreFloat3(&position, pos);
	positionVector = DirectX::XMLoadFloat3(&position);
	UpdateViewMatrix();
}

void Camera::SetPosition( const DirectX::XMFLOAT3 pos)
{
	position = pos;
	positionVector = DirectX::XMLoadFloat3(&position);
	UpdateViewMatrix();
}

void Camera::Move(DirectX::XMVECTOR& pos)
{
	DirectX::XMFLOAT3 temp;
	DirectX::XMStoreFloat3(&temp, pos);
	temp.x = position.x + temp.x;
	temp.y = position.y + temp.y;
	temp.z = position.z + temp.z;

	positionVector = DirectX::XMLoadFloat3(&temp);
	position = temp;
	UpdateViewMatrix();
}

void Camera::Move( DirectX::XMFLOAT3 pos)
{
	position.x += pos.x;
	position.y += pos.y;
	position.z += pos.z;
	positionVector = DirectX::XMLoadFloat3(&position);
	UpdateViewMatrix();
}

void Camera::SetRotation( DirectX::XMVECTOR& rot)
{
	DirectX::XMStoreFloat3(&rotation, rot);
	rotationVector = DirectX::XMLoadFloat3(&rotation);
	UpdateViewMatrix();
}

void Camera::SetRotation( DirectX::XMFLOAT3 rot)
{
	rotation = rot;
	rotationVector = DirectX::XMLoadFloat3(&rotation);
	UpdateViewMatrix();
}

void Camera::Rotate( DirectX::XMVECTOR& rot)
{
	DirectX::XMFLOAT3 temp;
	DirectX::XMStoreFloat3(&temp, rot);
	temp.x = rotation.x + temp.x;
	temp.y = rotation.y + temp.y;
	temp.z = rotation.z + temp.z;

	rotationVector = DirectX::XMLoadFloat3(&temp);
	rotation = temp;
	UpdateViewMatrix();
}

void Camera::Rotate( DirectX::XMFLOAT3 rot)
{
	rotation.x += rot.x;
	rotation.y += rot.y;
	rotation.z += rot.z;
	positionVector = DirectX::XMLoadFloat3(&rotation);
	UpdateViewMatrix();
}

void Camera::SetLookAtPos(DirectX::XMFLOAT3 lookAtPos)
{
	if (lookAtPos.x == position.x && lookAtPos.y == position.y && lookAtPos.z == position.z) {
		return;
	}

	lookAtPos.x = position.x - lookAtPos.x;
	lookAtPos.y = position.y - lookAtPos.y;
	lookAtPos.z = position.z - lookAtPos.z;

	float pitch = 0.0f;
	if (lookAtPos.y != 0.0f)
	{
		const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
		pitch = atan(lookAtPos.y / distance);
	}

	float yaw = 0.0f;
	if (lookAtPos.x != 0.0f)
	{
		yaw = atan(lookAtPos.x / lookAtPos.z);
	}
	if (lookAtPos.z > 0)
		yaw += DirectX::XM_PI;

	SetRotation(DirectX::XMFLOAT3(pitch, yaw, 0.0f));
}

 DirectX::XMVECTOR& Camera::GetForwardVector()
{
	return forward;
}

 DirectX::XMVECTOR& Camera::GetRightVector()
{
	return right;
}

 DirectX::XMVECTOR& Camera::GetBackwardVector()
{
	return backward;
}

 DirectX::XMVECTOR& Camera::GetLeftVector()
{
	return left;
}

void Camera::UpdateViewMatrix()
{
	//Calculate camera rotation matrix
	DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	//Calculate unit vector of cam target based off camera forward value transformed by cam rotation matrix
	DirectX::XMVECTOR camTarget = DirectX::XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, camRotationMatrix);
	//Adjust cam target to be offset by the camera's current position
	DirectX::XMFLOAT3 temp1, temp2;
	DirectX::XMStoreFloat3(&temp1, camTarget);
	DirectX::XMStoreFloat3(&temp2, positionVector);
	temp1.x += temp2.x;
	temp1.y += temp2.y;
	temp1.z += temp2.z;
	camTarget = DirectX::XMLoadFloat3(&temp1);
	//Calculate up direction based on current rotation
	DirectX::XMVECTOR upDir = DirectX::XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, camRotationMatrix);
	//Rebuild view matrix
	this->viewMatrix = DirectX::XMMatrixLookAtLH(positionVector, camTarget, upDir);

	DirectX::XMMATRIX vecRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(0.0f, rotation.y, 0.0f);
	forward = DirectX::XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
	backward = DirectX::XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
	left = DirectX::XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vecRotationMatrix);
	right = DirectX::XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vecRotationMatrix);
}
