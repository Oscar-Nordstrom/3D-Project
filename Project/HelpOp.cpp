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

void operator+=(XMFLOAT3& a, XMFLOAT3 b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
}

float operator*(XMFLOAT3 a, XMFLOAT3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
XMFLOAT3 operator*(XMFLOAT3 a, XMFLOAT3X3 b)
{
	XMFLOAT3 ret;
	ret.x = (a.x * b._11 + a.y * b._21 + a.z * b._31);
	ret.y = (a.x * b._12 + a.y * b._22 + a.z * b._32);
	ret.z = (a.x * b._13 + a.y * b._23 + a.z * b._33);
	return ret;
}

XMFLOAT3 operator*(XMFLOAT3X3 a, XMFLOAT3 b)
{
	XMFLOAT3 ret;
	ret.x = (a._11 * b.x + a._12 * b.y + a._13 * b.z);
	ret.x = (a._21 * b.x + a._22 * b.y + a._23 * b.z);
	ret.x = (a._31 * b.x + a._32 * b.y + a._33 * b.z);
	return ret;
}

bool operator==(const XMFLOAT3& a, const XMFLOAT3& b)
{
	if (a.x == b.x && a.y == b.z && a.z == b.z) {
		return true;
	}
	return false;
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
