#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "Frustum.h"


class Camera {
public:
	Camera();
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

	void Rotate(float rot);

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
};