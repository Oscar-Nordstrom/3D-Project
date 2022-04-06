#include "Mouse.h"

Mouse::Mouse()
	:x(0), y(0)
{
}

Mouse::~Mouse()
{
}

void Mouse::OnMouseMove(int newX, int newY)
{
	x = newX;
	y = newY;
}

int Mouse::GetPosX() const
{
	return x;
}

int Mouse::GetPosY() const
{
	return y;
}
