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
/*private:
    float operator*(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    DirectX::XMFLOAT3 operator*(DirectX::XMFLOAT3 a, float b) {
        return XMFLOAT3(a.x * b, a.y * b, a.z * b);
    }
    DirectX::XMFLOAT3 operator*(float a, DirectX::XMFLOAT3 b) {
        return XMFLOAT3(b.x * a, b.y * a, b.z * a);
    }
    DirectX::XMFLOAT3 operator+(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b) {
        return XMFLOAT3(a.x + b.x, a.y + b.y, a.z + b.z);
    }
    DirectX::XMFLOAT3 operator-(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b) {
        return XMFLOAT3(a.x - b.x, a.y - b.y, a.z - b.z);
    }
    DirectX::XMFLOAT3 operator-(DirectX::XMFLOAT3 a) {
        return XMFLOAT3(-a.x, -a.y, -a.z);
    }
    DirectX::XMFLOAT3 cross(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b) {
        XMFLOAT3 result;
        result.x = (a.y * b.z) - (a.z * b.y);
        result.y = (a.z * b.x) - (a.x * b.z);
        result.z = (a.x * b.y) - (a.y * b.x);
        return result;
    }
    float length(DirectX::XMFLOAT3 a) {
        return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
    }
    DirectX::XMFLOAT3 normalize(DirectX::XMFLOAT3 a) {
        float len = length(a);
        return XMFLOAT3(a.x / len, a.y / len, a.z / len);
    }*/
};