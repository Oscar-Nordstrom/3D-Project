#pragma once

class Mouse {
public:
	Mouse();
	~Mouse();

	void OnMouseMove(int newX, int newY);

	int GetPosX()const;
	int GetPosY()const;
private:
	int x, y;
};