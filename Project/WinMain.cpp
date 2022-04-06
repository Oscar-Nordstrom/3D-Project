#include "Scene.h"

#define STB_IMAGE_IMPLEMENTATION  
#include "stb_image.h"

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	Scene scene;
	return scene.Start();
}