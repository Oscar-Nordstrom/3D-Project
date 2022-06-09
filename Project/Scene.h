#pragma once
#include "Window.h"
#include "Timer.h"
#include "Camera.h"

#include <math.h>


#include "Model.h"
#include "SceneObjectTest.h"
#include "QuadTree.h"

#include "Structures.h"
#include "ShadowMap.h"
#include "CubeMap.h"
#include <string>
#include <iostream>
#include <DirectXMath.h>

#include "flags.h"

#if _DEBUG
#define NUM_SOLDIERS 4
#else 
#define NUM_SOLDIERS 27
#endif

class Scene {
public:
	Scene();
	~Scene();
	int Start();
private:
	bool DoFrame();
	bool SetUpDirLight();
	bool SetUpSpotLighs();
	bool SetUpBufs();
	void UpdateBufs();
	bool UpdateObjcects(float t);

	void checkInput();
	void cubeMapSetCam(int num);

	void SetLights();
	void HandleLightSettings();

	void ImGuiWindows();

	void EnableTesselation();
	void DisableTesselation();
	void HandleCulling(Camera& cam);

	void SetUpGameObjects();

	void UpdateCamera();
	void UpdateProjection();

	void UpdateMouseDelta();

	TextureHandler* texHandl = new TextureHandler();
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
	
	SceneObjectTest soldiers[NUM_SOLDIERS];
	SceneObjectTest ground , ground1, ground2, ground3;
	std::vector<SceneObjectTest*> grounds;
	std::vector<SceneObjectTest*>gameObjects;
	DirectionalLight dLight;
	SpotLight sLights[3];
	ShadowMap shadow;
	ShadowShaderBuffer shadowBufferData[NUM_LIGHTS];
	ID3D11Buffer* shadowMapBufs[NUM_LIGHTS];

	ID3D11Buffer* lightBuf;
	ID3D11Buffer* lightBufSpots;
	ID3D11Buffer* camBuf;
	ID3D11Buffer* camBuf2;
	ID3D11Buffer* camBuf3;
	ID3D11Buffer* camBufCubeMap;
	ID3D11Buffer* camBuf2CubeMap;
	ID3D11Buffer* camBuf3CubeMap;
	ID3D11Buffer* camBufTime;
	ID3D11Buffer* shadowSettings;

	QuadTree* qtree;
	std::vector<QuadTree*> intersectingNodes;
	std::vector<SceneObjectTest*>objectsToDraw;

	DirectX::XMMATRIX proj;

	//Settings
	bool tesselation;
	bool tesselationTemp;
	bool quadTreeOn;
	bool frustumCheckOn;
	bool shadowsOn;
	bool particlesOn;
	bool cubeMappingOn;
	bool sLight1On;
	bool sLight2On;
	bool sLight3On;

	bool updateCulling;

	float nearZ;
	float farZ;
	float fov;

	float mouseDX, mouseDY, mouseXtemp, mouseYtemp;
};