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
	//In radians
	const XMVECTOR& GetRotationVector()const;
	//In radians
	const XMFLOAT3& GetRotationFloat3()const;
	const XMVECTOR& GetForwardVec();
	const XMVECTOR& GetBackVec();
	const XMVECTOR& GetUpVec();
	const XMVECTOR& GetDownVec();
	const XMVECTOR& GetLeftVec();
	const XMVECTOR& GetRightVec();
	const XMFLOAT3& GetForwardFloat3();
	const XMFLOAT3& GetBackFloat3();
	const XMFLOAT3& GetUpFloat3();
	const XMFLOAT3& GetDownFloat3();
	const XMFLOAT3& GetLeftFloat3();
	const XMFLOAT3& GetRightFloat3();
	float* GetFrustumBuffer();
	Frustum* GetFrustum();
	float& GetSpeed();
	float& GetRotSpeed();

	void SetPosition( XMVECTOR& pos);
	void SetPosition( XMFLOAT3& pos);
	void SetPosition(float x, float y, float z);
	void Move(const XMVECTOR& pos);
	void Move(const XMFLOAT3& pos);
	void Move(float x, float y, float z);
	void SetRotationRad(const XMVECTOR& rot);
	void SetRotationRad(const XMFLOAT3& rot);
	void SetRotationRad(float x, float y, float z);
	void RotateRad( XMVECTOR& rot);
	void RotateRad( XMFLOAT3& rot);
	void RotateRad(float x, float y, float z);
	void SetRotationDeg(const XMVECTOR& rot);
	void SetRotationDeg(const XMFLOAT3& rot);
	void SetRotationDeg(float x, float y, float z);
	void RotateDeg( XMVECTOR& rot);
	void RotateDeg( XMFLOAT3& rot);
	void RotateDeg(float x, float y, float z);

	bool Intersect(DirectX::BoundingBox box);
	bool Intersect(DirectX::BoundingSphere sphere);

private:
	void UpdateViewMatrix();
	void UpdateFrustum();
	void CheckRotation(XMVECTOR& rot);
	void CheckRotation(XMFLOAT3& rot);
	void CheckRotation(float& x, float& y, float& z);
	void CheckRotationDeg(XMVECTOR& rot);
	void CheckRotationDeg(XMFLOAT3& rot);
	void CheckRotationDeg(float& x, float& y, float& z);
	XMVECTOR posVector;
	XMVECTOR rotVector;
	XMFLOAT3 pos;
	XMFLOAT3 rot;
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;

	Frustum frustum;
	DirectX::BoundingFrustum testFrustum;
	float speed, rotSpeed, nearZ, farZ, width, height, fov;

	const XMVECTOR DefaultForwardVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const XMVECTOR DefaultBackVector = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const XMVECTOR DefaultUpVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const XMVECTOR DefaultDownVector = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	const XMVECTOR DefaultLeftVector = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const XMVECTOR DefaultRightVector = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	XMVECTOR forward,back,left,right,up,down;
	XMFLOAT3 forwardFloat3, backFloat3, leftFloat3, rightFloat3, upFloat3, downFloat3;

};