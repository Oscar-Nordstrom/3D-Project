#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <vector>

#include "HelpOp.h"

struct Plane {
	DirectX::XMFLOAT3 point;
	DirectX::XMFLOAT3 normal;
};

class Frustum {
public:
	Frustum();
	~Frustum();
	void SetFrustum(DirectX::XMFLOAT3 camPos, float nearZ, float farZ, float width, float height, float fov, DirectX::XMFLOAT3 forward, DirectX::XMFLOAT3 up, DirectX::XMFLOAT3 right);
    bool intersect(DirectX::BoundingBox box);
	bool intersect(DirectX::BoundingSphere sphere);
    float* GetBuffer();
private:
    DirectX::BoundingFrustum frustumCheck;
	DirectX::XMFLOAT3 point;
	DirectX::XMFLOAT3 upLeft, upRight, downLeft, downRight;
	Plane nearPlane, farPlane, topPlane, rightPlane, bottomPlane, leftPlane;
	std::vector<Plane*>planes;
    float buffer;
};