#include "Frustum.h"

Frustum::Frustum()
{
	this->buffer = 1.0f;
}

Frustum::~Frustum()
{
}

void Frustum::SetFrustum(DirectX::XMFLOAT3 camPos, float nearZ, float farZ, float width, float height, float fov, DirectX::XMFLOAT3 forward, DirectX::XMFLOAT3 up, DirectX::XMFLOAT3 right)
{
	float aspect = width / height;
	width = nearZ * (float)tan(fov / 2) * aspect;
	height = nearZ * (float)tan(fov / 2);
	this->point = camPos;
	this->nearPlane.point = camPos + (nearZ * forward);
	this->farPlane.point = camPos + (farZ * forward);
	this->nearPlane.normal = -forward;
	this->farPlane.normal = forward;

	this->leftPlane.point = camPos;
	this->rightPlane.point = camPos;
	this->topPlane.point = camPos;
	this->bottomPlane.point = camPos;

	//buffer is 0 as default, but can be adjusten frim ImGui
	this->upLeft = camPos + (forward * nearZ) + (up * (height + buffer / 2)) - (right * (width + buffer / 2));
	this->upRight = camPos + (forward * nearZ) + (up * (height + buffer / 2)) + (right * (width + buffer / 2));
	this->downLeft = camPos + (forward * nearZ) - (up * (height + buffer / 2)) - (right * (width + buffer / 2));
	this->downRight = camPos + (forward * nearZ) - (up * (height + buffer / 2)) + (right * (width + buffer / 2));

	this->leftPlane.normal = normalize(cross(normalize(upLeft - camPos), up));
	this->rightPlane.normal = normalize(cross(up, normalize(upRight - camPos)));
	this->topPlane.normal = normalize(cross(-right, normalize(upLeft - camPos)));
	this->bottomPlane.normal = normalize(cross(right, normalize(downLeft - camPos)));

	planes.clear();
	planes.push_back(&nearPlane);
	planes.push_back(&farPlane);
	planes.push_back(&leftPlane);
	planes.push_back(&topPlane);
	planes.push_back(&rightPlane);
	planes.push_back(&bottomPlane);

}

bool Frustum::intersect(DirectX::BoundingBox box)
{
	DirectX::XMFLOAT3 points[8];
	float dist;
	bool allOut;
	for (auto p : planes) {
		allOut = true;
		box.GetCorners(points);
		for (int i = 0; i < 8; i++) {
			dist = p->normal * (points[i] - p->point);
			if (dist < 0) {
				allOut = false;
			}
		}
		if (allOut) {
			return false;
		}
	}

	//At least one point inside the frustum
	return true;
}

bool Frustum::intersect(DirectX::BoundingSphere sphere)
{
	for (auto p : planes) {
		float dist = p->normal * (sphere.Center - p->point);
		if (dist > sphere.Radius) {
			return false;
		}
	}
	return true;
}

float* Frustum::GetBuffer()
{
	return &this->buffer;
}

