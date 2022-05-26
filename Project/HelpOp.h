#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <math.h>

using namespace DirectX;

XMFLOAT3 operator*(float a, XMFLOAT3 b);
XMFLOAT3 operator*(XMFLOAT3 a, float b);
XMFLOAT3 operator/(XMFLOAT3 a, float b);
XMFLOAT3 operator+(XMFLOAT3 a, XMFLOAT3 b);
XMFLOAT3 operator-(XMFLOAT3 a);
XMFLOAT3 operator-(XMFLOAT3 a, XMFLOAT3 b);
void operator+=(XMFLOAT3& a, XMFLOAT3 b);
float operator*(XMFLOAT3 a, XMFLOAT3 b);
XMFLOAT3 operator*(XMFLOAT3 a, XMFLOAT3X3 b);
XMFLOAT3 operator*(XMFLOAT3X3 a, XMFLOAT3 b);

bool operator==(const XMFLOAT3& a, const XMFLOAT3& b);

XMFLOAT3 cross(XMFLOAT3 a, XMFLOAT3 b);
float length(XMFLOAT3 a);
XMFLOAT3 normalize(XMFLOAT3 a);
