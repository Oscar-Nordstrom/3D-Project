#include "HelpOp.h"

XMFLOAT3 operator*(float a, XMFLOAT3 b)
{
	return XMFLOAT3(b.x * a, b.y * a, b.z * a);
}

XMFLOAT3 operator*(XMFLOAT3 a, float b)
{
	return XMFLOAT3(a.x * b, a.y * b, a.z * b);
}

XMFLOAT3 operator/(XMFLOAT3 a, float b)
{
	return XMFLOAT3(a.x / b, a.y / b, a.z / b);
}

XMFLOAT3 operator+(XMFLOAT3 a, XMFLOAT3 b)
{
	return XMFLOAT3(a.x + b.x, a.y + b.y, a.z + b.z);
}

XMFLOAT3 operator-(XMFLOAT3 a)
{
	return XMFLOAT3(-a.x, -a.y, -a.z);
}

XMFLOAT3 operator-(XMFLOAT3 a, XMFLOAT3 b)
{
	return XMFLOAT3(a.x - b.x, a.y - b.y, a.z - b.z);
}

float operator*(XMFLOAT3 a, XMFLOAT3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

XMFLOAT3 cross(XMFLOAT3 a, XMFLOAT3 b) {
	XMFLOAT3 result;
	result.x = (a.y * b.z) - (a.z * b.y);
	result.y = (a.z * b.x) - (a.x * b.z);
	result.z = (a.x * b.y) - (a.y * b.x);
	return result;
}

float length(XMFLOAT3 a) {
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

XMFLOAT3 normalize(XMFLOAT3 a) {
	float len = length(a);
	return XMFLOAT3(a.x / len, a.y / len, a.z / len);
}
