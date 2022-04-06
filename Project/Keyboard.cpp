#include "Keyboard.h"

Keyboard::Keyboard()
{
	for (int i = 0; i < NUM_KEYS; i++) {
		keys[i] = false;
	}
}

Keyboard::~Keyboard()
{
}

bool Keyboard::KeyIsPressed(unsigned char keycode)
{
	int k = int(keycode);
	if (k < 256 && k >= 0) {
		return keys[k];
	}
	return false;
}

void Keyboard::Keydown(unsigned char keycode)
{
	int k = int(keycode);
	if (!keys[k]) {
		keys[k] = true;
	}
}

void Keyboard::KeyUp(unsigned char keycode)
{
	int k = int(keycode);
	if (keys[k]) {
		keys[k] = false;
	}
}

