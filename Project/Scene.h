#pragma once
#include "Window.h"
#include "Timer.h"
#include "Camera.h"

#include <math.h>


#include "Model.h"
#include "SceneObjectTest.h"

#include "SceneObject.h"
#include "Structures.h"
#include "ShadowMap.h"
#include "CubeMap.h"
#include <string>
#include <iostream>
#include <DirectXMath.h>

#include "flags.h"

class Scene {
public:
	Scene();
	~Scene();
	int Start();
private:
	bool DoFrame();
	bool SetUpDirLight();
	bool SetUpBufs();
	void UpdateBufs();
	bool UpdateObjcects(float t);

	void checkInput();
	void cubeMapSetCam(int num);

	void SetUpSkybox();

	void ImGuiWindows();

	float DegToRad(float deg);

	void EnableTesselation();
	void DisableTesselation();

	float speedfactor = 0.0f;
	float timerCount = 0.0f;
	float dt;
	TimeData theTimedata;
	Window window;
	Timer timer;
	Camera cam;
	CubeMap cMap;
	SceneObjectTest particle;
	SceneObjectTest cube;
	//SceneObjectTest ground;
	SceneObjectTest soldier1;
	SceneObjectTest soldier2;
	SceneObjectTest soldier3;
	SceneObjectTest soldier4;
	SceneObjectTest soldier5;
	SceneObjectTest soldier6;
	std::vector<SceneObjectTest*>gameObjects;
	DirectionalLight dLight;
	ShadowMap shadow;
	ID3D11Buffer* lightBuf;
	ID3D11Buffer* camBuf;
	ID3D11Buffer* camBuf2;
	ID3D11Buffer* camBufTime;

	vector<SceneObjectTest*> skybox;

	DirectX::XMMATRIX proj;

	bool tesselation;
	bool tesselationTemp;

};