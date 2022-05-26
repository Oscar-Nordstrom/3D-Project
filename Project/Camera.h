#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "Frustum.h"

using namespace DirectX;

class Camera {
public:
	Camera();
	~Camera();
	void SetProj(float fov, int width, int height, float nearZ, float farZ);
	const XMMATRIX& GettViewMatrix()const;
	const XMMATRIX& GettProjectionMatrix()const;
	const XMVECTOR& GetPositionVector()const;
	const XMFLOAT3& GetPositionFloat3()const;
	const XMVECTOR& GetRotationVector()const;
	const XMFLOAT3& GetRotationFloat3()const;
	const XMVECTOR& GetForwardVec();
	const XMVECTOR& GetBackVec();
	const XMVECTOR& GetUpVec();
	const XMVECTOR& GetDownVec();
	const XMVECTOR& GetLeftVec();
	const XMVECTOR& GetRightVec();
	float* GetFrustumBuffer();
	Frustum* GetFrustum();
	float& GetSpeed();
	float& GetRotSpeed();

	void SetPosition(const XMVECTOR& pos);
	void SetPosition(const XMFLOAT3& pos);
	void SetPosition(float x, float y, float z);
	void Move(const XMVECTOR& pos);
	void Move(const XMFLOAT3& pos);
	void Move(float x, float y, float z);
	void SetRotation(const XMVECTOR& rot);
	void SetRotation(const XMFLOAT3& rot);
	void SetRotation(float x, float y, float z);
	void Rotate(const XMVECTOR& rot);
	void Rotate(const XMFLOAT3& rot);
	void Rotate(float x, float y, float z);

	bool Intersect(DirectX::BoundingBox box);
	bool Intersect(DirectX::BoundingSphere sphere);

private:
	void UpdateViewMatrix();
	void UpdateFrustum();
	XMVECTOR posVector;
	XMVECTOR rotVector;
	XMFLOAT3 pos;
	XMFLOAT3 rot;
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;

	Frustum frustum;
	float speed, rotSpeed, nearZ, farZ, width, height, fov;

	const XMVECTOR DefaultForwardVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const XMVECTOR DefaultBackVector = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const XMVECTOR DefaultUpVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const XMVECTOR DefaultDownVector = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	const XMVECTOR DefaultLeftVector = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const XMVECTOR DefaultRightVector = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	XMVECTOR forward,back,left,right,up,down;

};

/*class Camera {
public:
	Camera();
	Camera(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 dir);
	~Camera();
	DirectX::XMMATRIX GetMatrix();
	DirectX::XMFLOAT3* GetPos();
	DirectX::XMFLOAT3* GetDir();
	//DirectX::BoundingFrustum GetFrustum();
	Frustum* GetFrustum();
	float* GetFrustumBuffer();
	DirectX::XMFLOAT3 GetForward();
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMFLOAT3 GetRight();
	float GetAngle();
	void SetPos(DirectX::XMFLOAT3 pos);
	void SetDir(DirectX::XMFLOAT3 dir);
	void SetUpDir(DirectX::XMFLOAT3 dir);
	void SetProjection(DirectX::XMMATRIX proj);
	void Reset();

	void RotateSide(float rot);
	void RotateVertical(float rot);

	void forward();
	void backward();
	void left();
	void right();
	void up();
	void down();

	void Update(float nearZ, float farZ, float width, float height, float fov);

	bool Intersect(DirectX::BoundingBox box);
	bool Intersect(DirectX::BoundingSphere sphere);

private:
	//void updateFrustum();
	float DegToRad(float deg);
private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 direction;
	DirectX::XMFLOAT3 upDirection;

	Frustum frustum;


	//DirectX::BoundingFrustum frustum;

public:
	float speed;
	float angle;
};*/