#pragma once

#define NUM_KEYS 256

class Keyboard {
public:
	Keyboard();
	~Keyboard();

	bool KeyIsPressed(unsigned char keycode);
	bool KeyIsPressed(int keycode);

	void Keydown(unsigned char keycode);
	void KeyUp(unsigned char keycode);
private:
	bool keys[NUM_KEYS];
};