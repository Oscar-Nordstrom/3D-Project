#include "Scene.h"
#include "imgui.h"

int roundUpTo(int numToRound, int multiple)
{
	if (multiple == 0)
		return numToRound;

	int remainder = numToRound % multiple;
	if (remainder == 0)
		return numToRound;

	return numToRound + multiple - remainder;
}

Scene::Scene()
	:window(WIDTH, HEIGHT, L"Project"), cMap(window.Gfx())
{

	tesselation = true;
	tesselationTemp = tesselation;
	quadTreeOn = false;
	frustumCheckOn = false;
	shadowsOn = true;
	particlesOn = true;
	cubeMappingOn = true;
	sLight1On = true;
	sLight2On = true;
	sLight3On = true;

	dt = 0;
	this->updateCulling = true;

	cam.SetPosition(0.0f, 0.0f, -3.0f);
	cam.SetProj(90.0f, window.GetWidth(), window.GetHeight(), 0.1f, 1000.0f);
	window.Gfx()->SetProjection(cam.GettProjectionMatrix());
	window.Gfx()->SetCamera(cam.GettViewMatrix());

	SetUpGameObjects();

	SetLights();

	SetUpBufs();

	shadow.Init(window.Gfx(), &dLight);
	shadow.SetSpotLights(sLights);

	qtree = new QuadTree(gameObjects, 0, 300.0f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));



	this->mouseXtemp = (float)window.mouse.GetPosX();
	this->mouseYtemp = (float)window.mouse.GetPosY();

	HandleCulling(cam);




}

Scene::~Scene()
{
	if (lightBuf)lightBuf->Release();
	if (lightBufSpots)lightBufSpots->Release();
	if (camBuf)camBuf->Release();
	if (camBuf2)camBuf2->Release();
	if (camBuf3)camBuf3->Release();
	if (camBufCubeMap)camBufCubeMap->Release();
	if (camBuf2CubeMap)camBuf2CubeMap->Release();
	if (camBuf3CubeMap)camBuf3CubeMap->Release();
	if (camBufTime)camBufTime->Release();
	if (shadowSettings)shadowSettings->Release();
	for (int i = 0; i < NUM_LIGHTS; i++) {
		if (shadowMapBufs[i])shadowMapBufs[i]->Release();
	}
	texHandl->Delete();
	delete texHandl;
	delete qtree;
}

int Scene::Start()
{
	while (true) {
		if (const auto code = window.ProcessMessages()) {
			return *code;
		}
		if (!DoFrame()) {
			std::cerr << "Failed to do frame.\n";
			return -1;
		}
		Sleep(1);
	}
}

bool Scene::DoFrame()
{

	const float t = timer.Mark() * speedfactor;
	dt = t;
	timerCount += t;
	theTimedata.dt = dt;
	theTimedata.time = timerCount;
	std::wstring timerString = L"Time elapsed " + std::to_wstring(timerCount);


	//std::wstring dirStr = L"X: " + std::to_wstring(cam.GetPositionFloat3().x) + L", Y: " + std::to_wstring(cam.GetPositionFloat3().y) + L", Z: " + std::to_wstring(cam.GetPositionFloat3().z);
	//std::wstring mouseString = L"Mouse pos: " + std::to_wstring(window.mouse.GetPosX()) + L", " + std::to_wstring(window.mouse.GetPosY());
	//std::wstring numObjectsString = L"Num objects: " + std::to_wstring(this->objectsToDraw.size());
	//std::wstring numNodesString = L"Num nodes: " + std::to_wstring(this->intersectingNodes.size());
	window.SetTitle(timerString.c_str());

	//Enable/Disable tesselation
	if (tesselationTemp != tesselation) {
		if (tesselation) {
			EnableTesselation();
		}
		else {
			DisableTesselation();
		}
		tesselationTemp = tesselation;
	}

	if (updateCulling) {
		HandleCulling(cam);
		this->updateCulling = false;
	}

	HandleLightSettings();

	//Cube mapping first Start
	if (cubeMappingOn) {


 		cMap.Clear(window.Gfx()->GetContext());
		//Go through all uavs
		for (int i = 0; i < NUM_TEX; i++) {
			//Rotate camera
			cubeMapSetCam(i);
			HandleCulling(cMap.GetCam());

			//Shadows Start First Cube Map
			DirectX::XMFLOAT3 tempPos = cam.GetPositionFloat3();
			DirectX::XMFLOAT3 tempDir = cam.GetRotationFloat3();
			shadow.SetSpotLights(sLights);
			shadow.SetDirLight(&dLight);
			shadow.StartFirst();

			for (int i = 0; i < NUM_LIGHTS; i++) {

				if (i > 0) {
					int index = i - 1;
					cam.SetPosition(sLights[index].position);
					cam.SetRotationDeg(90, 0.0f, 0.0f);
					shadow.UpdateWhatShadow(i, SPOT_LIGHT);//Sets depth
				}
				else {
					cam.SetPosition(0.0f, 40.0f, 0.0f);
					cam.SetRotationDeg(90, 0.0f, 0.0f);
					shadow.UpdateWhatShadow(i, DIRECTIONAL_LIGHT);//Sets depth
				}
				UpdateCamera();
				shadowBufferData[i].view = cam.GettViewMatrix();
				shadowBufferData[i].proj = window.Gfx()->GetProjection();
				window.Gfx()->StartFrame(0.0f, 0.0f, 0.0f, SHADOW);
				if (shadowsOn) {
					for (auto p : objectsToDraw) {
						p->Draw(window.Gfx(), SHADOW);
					}
					if (cam.GetFrustum()->intersect(cube.GetBoundingSphere())) {
						cube.Draw(window.Gfx(), SHADOW);
					}
				}
			}

			cam.SetPosition(tempPos);
			cam.SetRotationRad(tempDir);
			UpdateCamera();
			//Shadows End First Cube Map


			//Render to current UAV
			window.Gfx()->StartFrame(0.0f, 0.0f, 0.0f, CUBE_MAP);
			shadow.StartSeccond();
			window.Gfx()->GetContext()->PSSetConstantBuffers(0, 1, &shadowSettings);
			window.Gfx()->GetContext()->PSSetConstantBuffers(1, 1, &lightBuf);
			window.Gfx()->GetContext()->PSSetConstantBuffers(2, 1, &lightBufSpots);
			window.Gfx()->GetContext()->PSSetConstantBuffers(3, 1, &camBuf);

			window.Gfx()->SetProjection(cMap.GetCam().GettProjectionMatrix());
			window.Gfx()->SetCamera(cMap.GetCam().GettViewMatrix());

			//Setting the new UAV
			cMap.Set(window.Gfx()->GetContext(), i);
			if (cam.GetFrustum()->intersect(cube.GetBoundingSphere())) {
				cube.Draw(window.Gfx(), CUBE_MAP);
			}
			
			for (auto p : objectsToDraw) {
				p->Draw(window.Gfx(), CUBE_MAP);
			}
			for (auto p : grounds) {
				p->Draw(window.Gfx(), CUBE_MAP);
			}

			if (particlesOn) {
				window.Gfx()->GetContext()->GSSetConstantBuffers(0, 1, &camBufCubeMap);
				window.Gfx()->GetContext()->GSSetConstantBuffers(1, 1, &camBuf2CubeMap);
				window.Gfx()->GetContext()->GSSetConstantBuffers(2, 1, &camBuf3CubeMap);
				particle.Draw(window.Gfx(), PARTICLE);
			}

			window.Gfx()->GetContext()->HSSetConstantBuffers(0, 1, &camBuf);
			window.Gfx()->GetContext()->CSSetConstantBuffers(1, 1, &lightBuf);
			window.Gfx()->GetContext()->CSSetConstantBuffers(2, 1, &camBuf);
			window.Gfx()->EndFrame(W_H_CUBE, W_H_CUBE, CUBE_MAP);
		}
		HandleCulling(cam);
	}
	//Cube map first end


	//Shadows Start First
	DirectX::XMFLOAT3 tempPos = cam.GetPositionFloat3();
	DirectX::XMFLOAT3 tempDir = cam.GetRotationFloat3();
	shadow.SetSpotLights(sLights);
	shadow.SetDirLight(&dLight);
	shadow.StartFirst();

	for (int i = 0; i < NUM_LIGHTS; i++) {

		if (i > 0) {
			int index = i - 1;
			cam.SetPosition(sLights[index].position);
			cam.SetRotationDeg(90, 0.0f, 0.0f);
			shadow.UpdateWhatShadow(i, SPOT_LIGHT);//Sets depth
		}
		else {
			cam.SetPosition(0.0f, 40.0f, 0.0f);
			cam.SetRotationDeg(90, 0.0f, 0.0f);
			shadow.UpdateWhatShadow(i, DIRECTIONAL_LIGHT);//Sets depth
		}
		UpdateCamera();
		shadowBufferData[i].view = cam.GettViewMatrix();
		shadowBufferData[i].proj = window.Gfx()->GetProjection();
		window.Gfx()->StartFrame(0.0f, 0.0f, 0.0f, SHADOW);
		if (shadowsOn) {
			for (auto p : objectsToDraw) {
				p->Draw(window.Gfx(), SHADOW);
			}
			if (cam.GetFrustum()->intersect(cube.GetBoundingSphere())) {
				cube.Draw(window.Gfx(), SHADOW);
			}
		}
	}

	cam.SetPosition(tempPos);
	cam.SetRotationRad(tempDir);
	UpdateCamera();
	//Shadows End First

	//Final draw Start
	window.Gfx()->StartFrame(0.0f, 0.0f, 0.0f);
	ImGuiWindows();
	UpdateProjection();
	UpdateCamera();
	UpdateBufs();
	checkInput();
	UpdateObjcects(t);

	//Cube map seccond Start
	if (cubeMappingOn) {
		cMap.SetSeccond(window.Gfx());
		cube.Draw(window.Gfx(), CUBE_MAP_TWO);
		cMap.SetEnd(window.Gfx());
	}
	//Cube map seccond End

	//Shadows Start Second
	window.Gfx()->GetContext()->PSSetConstantBuffers(0, 1, &shadowSettings);
	window.Gfx()->GetContext()->DSSetConstantBuffers(1, NUM_LIGHTS, shadowMapBufs);
	window.Gfx()->GetContext()->PSSetConstantBuffers(1, 1, &lightBuf);
	window.Gfx()->GetContext()->PSSetConstantBuffers(2, 1, &lightBufSpots);
	shadow.StartSeccond();
	//Shadows End Second

	for (auto p : objectsToDraw) {
		p->Draw(window.Gfx());
	}

	for (auto p : grounds) {
		p->Draw(window.Gfx());
	}
	//Particle Start
	if (particlesOn) {
		window.Gfx()->GetContext()->GSSetConstantBuffers(0, 1, &camBuf);
		window.Gfx()->GetContext()->GSSetConstantBuffers(1, 1, &camBuf2);
		window.Gfx()->GetContext()->GSSetConstantBuffers(2, 1, &camBuf3);
		particle.Draw(window.Gfx(), PARTICLE);
	}
	//Particle End

	//dLight;
	window.Gfx()->GetContext()->HSSetConstantBuffers(0, 1, &camBuf);
	window.Gfx()->GetContext()->CSSetConstantBuffers(1, 1, &lightBuf);
	window.Gfx()->GetContext()->CSSetConstantBuffers(2, 1, &camBuf);
	window.Gfx()->GetContext()->CSSetConstantBuffers(3, 1, &camBuf2);
	window.Gfx()->GetContext()->CSSetConstantBuffers(4, 1, &lightBufSpots);
	window.Gfx()->EndFrame(window.GetWidth(), window.GetHeight());
	//Final draw End

	//Update particles
	if (particlesOn) {
		particle.UpdateParticle(window.Gfx());
		window.Gfx()->GetContext()->CSSetConstantBuffers(3, 1, &camBufTime);
		window.Gfx()->UpdateParticles();
	}

	return true;
}

bool Scene::SetUpDirLight()
{
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = roundUpTo(sizeof(dLight), 16);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &dLight;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = window.Gfx()->GetDevice()->CreateBuffer(&desc, &data, &lightBuf);

	return !FAILED(hr);
}

bool Scene::SetUpSpotLighs()
{
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = roundUpTo(sizeof(SpotLight) * 3, 16);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &sLights;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = window.Gfx()->GetDevice()->CreateBuffer(&desc, &data, &lightBufSpots);

	return !FAILED(hr);
}

bool Scene::SetUpBufs()
{
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = roundUpTo(sizeof(cam.GetPositionFloat3()), 16);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &cam.GetPositionFloat3();
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = window.Gfx()->GetDevice()->CreateBuffer(&desc, &data, &camBuf);
	if (FAILED(hr)) {
		return false;
	}

	desc.ByteWidth = roundUpTo(sizeof(cMap.GetCam().GetForwardFloat3()), 16);
	data.pSysMem = &cMap.GetCam().GetForwardFloat3();
	hr = window.Gfx()->GetDevice()->CreateBuffer(&desc, &data, &camBufCubeMap);
	if (FAILED(hr)) {
		return false;
	}

	desc.ByteWidth = roundUpTo(sizeof(cam.GetForwardFloat3()), 16);
	data.pSysMem = &cam.GetForwardFloat3();
	hr = window.Gfx()->GetDevice()->CreateBuffer(&desc, &data, &camBuf2);
	if (FAILED(hr)) {
		return false;
	}

	desc.ByteWidth = roundUpTo(sizeof(cMap.GetCam().GetForwardFloat3()), 16);
	data.pSysMem = &cMap.GetCam().GetForwardFloat3();
	hr = window.Gfx()->GetDevice()->CreateBuffer(&desc, &data, &camBuf2CubeMap);
	if (FAILED(hr)) {
		return false;
	}

	desc.ByteWidth = roundUpTo(sizeof(cam.GetUpFloat3()), 16);
	data.pSysMem = &cam.GetUpFloat3();
	hr = window.Gfx()->GetDevice()->CreateBuffer(&desc, &data, &camBuf3);
	if (FAILED(hr)) {
		return false;
	}

	desc.ByteWidth = roundUpTo(sizeof(cMap.GetCam().GetUpFloat3()), 16);
	data.pSysMem = &cMap.GetCam().GetUpFloat3();
	hr = window.Gfx()->GetDevice()->CreateBuffer(&desc, &data, &camBuf3CubeMap);
	if (FAILED(hr)) {
		return false;
	}

	for (int i = 0; i < NUM_LIGHTS; i++) {
		desc.ByteWidth = roundUpTo(sizeof(shadowBufferData[i]), 16);
		data.pSysMem = &shadowBufferData[i];
		hr = window.Gfx()->GetDevice()->CreateBuffer(&desc, &data, &shadowMapBufs[i]);
		if (FAILED(hr)) {
			return false;
		}
	}

	desc.ByteWidth = roundUpTo(sizeof(bool), 16);
	data.pSysMem = &shadowsOn;
	hr = window.Gfx()->GetDevice()->CreateBuffer(&desc, &data, &shadowSettings);
	if (FAILED(hr)) {
		return false;
	}

	theTimedata.dt = dt;
	theTimedata.time = timerCount;
	desc.ByteWidth = roundUpTo(sizeof(TimeData), 16);
	data.pSysMem = &dt;
	hr = window.Gfx()->GetDevice()->CreateBuffer(&desc, &data, &camBufTime);

	return !FAILED(hr);
}

void Scene::UpdateBufs()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;//Create a mapped resource
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));//Clear the mappedResource
	HRESULT hr = window.Gfx()->GetContext()->Map(camBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);//Disable GPU access to the data
	CopyMemory(mappedResource.pData, &cam.GetPositionFloat3(), sizeof(DirectX::XMFLOAT3));//Write the new memory
	window.Gfx()->GetContext()->Unmap(camBuf, 0);//Reenable GPU access to the data
	if (FAILED(hr)) {
		assert(false && "Failed to update buffer.");
	}

	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));//Clear the mappedResource
	hr = window.Gfx()->GetContext()->Map(camBufCubeMap, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);//Disable GPU access to the data
	CopyMemory(mappedResource.pData, &cMap.GetCam().GetPositionFloat3(), sizeof(DirectX::XMFLOAT3));//Write the new memory
	window.Gfx()->GetContext()->Unmap(camBufCubeMap, 0);//Reenable GPU access to the data
	if (FAILED(hr)) {
		assert(false && "Failed to update buffer.");
	}

	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));//Clear the mappedResource
	hr = window.Gfx()->GetContext()->Map(camBuf2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);//Disable GPU access to the data
	CopyMemory(mappedResource.pData, &cam.GetForwardFloat3(), sizeof(DirectX::XMFLOAT3));//Write the new memory
	window.Gfx()->GetContext()->Unmap(camBuf2, 0);//Reenable GPU access to the data
	if (FAILED(hr)) {
		assert(false && "Failed to update buffer.");
	}

	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));//Clear the mappedResource
	hr = window.Gfx()->GetContext()->Map(camBuf2CubeMap, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);//Disable GPU access to the data
	CopyMemory(mappedResource.pData, &cMap.GetCam().GetForwardFloat3(), sizeof(DirectX::XMFLOAT3));//Write the new memory
	window.Gfx()->GetContext()->Unmap(camBuf2CubeMap, 0);//Reenable GPU access to the data
	if (FAILED(hr)) {
		assert(false && "Failed to update buffer.");
	}

	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));//Clear the mappedResource
	hr = window.Gfx()->GetContext()->Map(camBuf3, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);//Disable GPU access to the data
	CopyMemory(mappedResource.pData, &cam.GetUpFloat3(), sizeof(DirectX::XMFLOAT3));//Write the new memory
	window.Gfx()->GetContext()->Unmap(camBuf3, 0);//Reenable GPU access to the data
	if (FAILED(hr)) {
		assert(false && "Failed to update buffer.");
	}

	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));//Clear the mappedResource
	hr = window.Gfx()->GetContext()->Map(camBuf3CubeMap, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);//Disable GPU access to the data
	CopyMemory(mappedResource.pData, &cMap.GetCam().GetUpFloat3(), sizeof(DirectX::XMFLOAT3));//Write the new memory
	window.Gfx()->GetContext()->Unmap(camBuf3CubeMap, 0);//Reenable GPU access to the data
	if (FAILED(hr)) {
		assert(false && "Failed to update buffer.");
	}

	for (int i = 0; i < NUM_LIGHTS; i++) {
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));//Clear the mappedResource
		hr = window.Gfx()->GetContext()->Map(shadowMapBufs[i], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);//Disable GPU access to the data
		CopyMemory(mappedResource.pData, &shadowBufferData[i], sizeof(ShadowShaderBuffer));//Write the new memory
		window.Gfx()->GetContext()->Unmap(shadowMapBufs[i], 0);//Reenable GPU access to the data
		if (FAILED(hr)) {
			assert(false && "Failed to update buffer.");
		}
	}

	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));//Clear the mappedResource
	hr = window.Gfx()->GetContext()->Map(lightBufSpots, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);//Disable GPU access to the data
	CopyMemory(mappedResource.pData, &sLights, sizeof(SpotLight)*3);//Write the new memory
	window.Gfx()->GetContext()->Unmap(lightBufSpots, 0);//Reenable GPU access to the data
	if (FAILED(hr)) {
		assert(false && "Failed to update buffer.");
	}

	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));//Clear the mappedResource
	hr = window.Gfx()->GetContext()->Map(lightBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);//Disable GPU access to the data
	CopyMemory(mappedResource.pData, &dLight, sizeof(dLight));//Write the new memory
	window.Gfx()->GetContext()->Unmap(lightBuf, 0);//Reenable GPU access to the data
	if (FAILED(hr)) {
		assert(false && "Failed to update buffer.");
	}

	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));//Clear the mappedResource
	hr = window.Gfx()->GetContext()->Map(camBufTime, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);//Disable GPU access to the data
	CopyMemory(mappedResource.pData, &theTimedata, sizeof(TimeData));//Write the new memory
	window.Gfx()->GetContext()->Unmap(camBufTime, 0);//Reenable GPU access to the data
	if (FAILED(hr)) {
		assert(false && "Failed to update buffer.");
	}

	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));//Clear the mappedResource
	hr = window.Gfx()->GetContext()->Map(shadowSettings, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);//Disable GPU access to the data
	CopyMemory(mappedResource.pData, &shadowsOn, sizeof(bool));//Write the new memory
	window.Gfx()->GetContext()->Unmap(shadowSettings, 0);//Reenable GPU access to the data
	if (FAILED(hr)) {
		assert(false && "Failed to update buffer.");
	}
}

bool Scene::UpdateObjcects(float t)
{

	for (int i = 0; i < NUM_SOLDIERS; i++) {
		if (!soldiers[i].Update(t, window.Gfx())) {
			std::cerr << "Failed to update object.\n";
			return false;
		}
	}
	for (auto p : grounds) {
		if (!p->Update(0.0f, window.Gfx())) {
			std::cerr << "Failed to update object.\n";
			return false;
		}
	}

	if (!cube.Update(0.0f, window.Gfx())) {
		std::cerr << "Failed to update object.\n";
		return false;
	}

	return true;
}

void Scene::checkInput()
{
	//Move
	if (window.Kbd()->KeyIsPressed('W')) {
		cam.Move(this->cam.GetForwardVec());
		this->updateCulling = true;
	}
	else if (window.Kbd()->KeyIsPressed('S')) {
		cam.Move(this->cam.GetBackVec());
		this->updateCulling = true;
	}
	if (window.Kbd()->KeyIsPressed('D')) {
		cam.Move(this->cam.GetRightVec());
		this->updateCulling = true;
	}
	else if (window.Kbd()->KeyIsPressed('A')) {
		cam.Move(this->cam.GetLeftVec());
		this->updateCulling = true;
	}
	if (window.Kbd()->KeyIsPressed(SPACE)) {
		cam.Move(0.0f, 1.0f, 0.0f);
		this->updateCulling = true;
	}
	else if (window.Kbd()->KeyIsPressed(CTRL)) {
		cam.Move(0.0f, -1.0f, 0.0f);
		this->updateCulling = true;
	}
	//Rotate
	if (window.Kbd()->KeyIsPressed(SHIFT)) {
		this->UpdateMouseDelta();
		if (this->mouseDX > 0.0f || this->mouseDX < 0.0f || this->mouseDY > 0.0f || this->mouseDY < 0.0f) {
			cam.RotateRad(this->mouseDY * 0.01f, this->mouseDX * 0.01f, 0.0f);
			this->mouseDX = 0.0f;
			this->mouseDY = 0.0f;
			this->updateCulling = true;
		}
	}
	else {
		this->mouseXtemp = (float)window.mouse.GetPosX();
		this->mouseYtemp = (float)window.mouse.GetPosY();
	}

}

void Scene::cubeMapSetCam(int num)
{
	//0: right
	//1: left
	//2: up
	//3: down
	//4: front
	//5: back
	/*switch (num)
	{
	case 0:
		cMap.GetCam().SetRotationDeg(0.0f, -90.0f, 0.0f);
		break;
	case 1:
		cMap.GetCam().SetRotationDeg(0.0f, 90.0f, 0.0f);
		break;
	case 2:
		cMap.GetCam().SetRotationDeg(-90.0f, 0.0f, 0.0f);
		break;
	case 3:
		cMap.GetCam().SetRotationDeg(90.0f, 0.0f, 0.0f);
		break;
	case 4:
		cMap.GetCam().SetRotationDeg(0.0f, 0.0f, 0.0f);
		break;
	case 5:
		cMap.GetCam().SetRotationDeg(0.0f, 180.0f, 0.0f);
		break;
	}*/
	switch (num)
	{
	case 0:
		cMap.GetCam().SetRotationDeg(0.0f, 90.0f, 0.0f);
		break;
	case 1:
		cMap.GetCam().SetRotationDeg(0.0f, -90.0f, 0.0f);
		break;
	case 2:
		cMap.GetCam().SetRotationDeg(-90.0f, 0.0f, 0.0f);
		break;
	case 3:
		cMap.GetCam().SetRotationDeg(90.0f, 0.0f, 0.0f);
		break;
	case 4:
		cMap.GetCam().SetRotationDeg(0.0f, 0.0f, 0.0f);
		break;
	case 5:
		cMap.GetCam().SetRotationDeg(0.0f, 180.0f, 0.0f);
		break;
	}
}

void Scene::SetLights()
{
	dLight.color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	dLight.direction = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
	dLight.on = true;
	SetUpDirLight();

	sLights[0].color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	sLights[1].color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	sLights[2].color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	sLights[0].position = { -150.0f, 20.0f, 50.0f };
	sLights[1].position = { -150.0f, 20.0f, 0.0f };
	sLights[2].position = { -150.0f, 20.0f, -50.0f };

	sLights[0].direction = { 0.0f, -1.0f, 0.0f };
	sLights[1].direction = { 0.0f, -1.0f, 0.0f };
	sLights[2].direction = { 0.0f, -1.0f, 0.0f };

	sLights[0].innerAngle = 15.0f;
	sLights[1].innerAngle = 15.0f;
	sLights[2].innerAngle = 15.0f;

	sLights[0].outerAngle = 20.0f;
	sLights[1].outerAngle = 20.0f;
	sLights[2].outerAngle = 20.0f;

	sLights[0].on = { 1.0f, 1.0f, 1.0f, 1.0f };
	sLights[1].on = { 1.0f, 1.0f, 1.0f, 1.0f };
	sLights[2].on = { 1.0f, 1.0f, 1.0f, 1.0f };
	SetUpSpotLighs();
}

void Scene::HandleLightSettings()
{
	if (sLight1On) {
		sLights[0].on = { 1.0f, 1.0f, 1.0f, 1.0f };
	}
	else {
		sLights[0].on = { 0.0f, 0.0f, 0.0f, 0.0f };
	}
	if (sLight2On) {
		sLights[1].on = { 1.0f, 1.0f, 1.0f, 1.0f };
	}
	else {
		sLights[1].on = { 0.0f, 0.0f, 0.0f, 0.0f };
	}
	if (sLight3On) {
		sLights[2].on = { 1.0f, 1.0f, 1.0f, 1.0f };
	}
	else {
		sLights[2].on = { 0.0f, 0.0f, 0.0f, 0.0f };
	}
}

void Scene::ImGuiWindows()
{
	if (ImGui::Begin("General Settings")) {
		ImGui::SliderFloat("Speed Factor", &speedfactor, 0.0f, 4.0f);
		ImGui::Checkbox("Tesselation", &tesselation);
		ImGui::Checkbox("QuadTree", &quadTreeOn);
		ImGui::Checkbox("Frustum Collision Check", &frustumCheckOn);
		ImGui::Checkbox("Shadows", &shadowsOn);
		ImGui::Checkbox("Particles", &particlesOn);
		ImGui::Checkbox("CubeMap", &cubeMappingOn);
	}
	ImGui::End();
	if (ImGui::Begin("Light Settings")) {
		ImGui::Checkbox("Directional Light", &dLight.on);
		ImGui::Checkbox("Spotlight 1 Light", &sLight1On);
		ImGui::SliderFloat("Spotlight 1 pos", &sLights[0].position.z, -50.0f, 50.0f);
		ImGui::Checkbox("Spotlight 2 Light", &sLight2On);
		ImGui::SliderFloat("Spotlight 2 pos", &sLights[1].position.z, -50.0f, 50.0f);
		ImGui::Checkbox("Spotlight 3 Light", &sLight3On);
		ImGui::SliderFloat("Spotlight 3 pos", &sLights[2].position.z, -50.0f, 50.0f);
	}
	ImGui::End();
	if (ImGui::Begin("Info")) {
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Intersecting nodes: %.0f", (float)intersectingNodes.size());
		ImGui::Text("Intersecting objects: %.0f", (float)objectsToDraw.size());
	}
	ImGui::End();
	if (ImGui::Begin("Player")) {
		ImGui::SliderFloat("Player Speed", &cam.GetSpeed(), 0.0f, 10.0f);
		ImGui::SliderFloat("Rotation Speed", &cam.GetRotSpeed(), 0.0f, 5.0f);
		ImGui::SliderFloat("Frustum buffer zone", cam.GetFrustumBuffer(), 0.0, 20.0f);
	}
	ImGui::End();
}

void Scene::EnableTesselation()
{
	for (SceneObjectTest* object : gameObjects) {
		object->EnableTesselation();
	}
}

void Scene::DisableTesselation()
{
	for (SceneObjectTest* object : gameObjects) {
		object->DisableTesselation();
	}
}

void Scene::HandleCulling(Camera& cam)
{
	if (quadTreeOn) {
		intersectingNodes.clear();
		for (auto p : objectsToDraw) {
			p->Remove();
		}
		objectsToDraw.clear();
		qtree->InsideNodes(cam, &intersectingNodes);
		for (auto p : intersectingNodes) {
			std::vector<SceneObjectTest*> vec = p->GetObjects();
			for (auto q : vec) {
				bool col = cam.GetFrustum()->intersect(q->GetBoundingSphere());
				if (!frustumCheckOn) {
					col = true;
				}
				if (!q->IsAdded() && col) {
					objectsToDraw.push_back(q);
					q->Add();
				}
			}
		}
	}
	else {
		objectsToDraw.clear();
		for (auto p : gameObjects) {
			objectsToDraw.push_back(p);
		}
	}
}

void Scene::SetUpGameObjects()
{
	std::string dir = window.Gfx()->GetShaderDir();
	for (int i = 0; i < NUM_SOLDIERS; i++) {
		soldiers[i].Init(texHandl, "../Resources/Obj/elite.obj", dir + "/VertexShader.cso", dir + "/HullShader.cso", dir + "/DomainShader.cso", dir + "/PixelShader.cso", dir + "/ComputeShader.cso", NO_SHADER, window.Gfx());
		soldiers[i].Scale(2.0f, 2.0f, 2.0f);
		gameObjects.push_back(&soldiers[i]);
	}
	soldiers[0].SetPos({ 40.0f, 0.0f, 40.0f });
	soldiers[1].SetPos({ -150.0f, -10.0f, 50.0f });
	soldiers[2].SetPos({ -150.0f, -5.0f, 0.0f });
	soldiers[3].SetPos({ -150.0f, 0.0f, -50.0f });

	if (NUM_SOLDIERS > 4) {
		soldiers[4].SetPos(DirectX::XMFLOAT3(30.0f, 0.0f, 30.0f));
		soldiers[5].SetPos(DirectX::XMFLOAT3(30.0f, 0.0f, -30.0f));
		soldiers[6].SetPos(DirectX::XMFLOAT3(-30.0f, 0.0f, 30.0f));
		soldiers[7].SetPos(DirectX::XMFLOAT3(-30.0f, 0.0f, -30.0f));

		soldiers[8].SetPos(DirectX::XMFLOAT3(20.0f, 0.0f, 20.0f));
		soldiers[9].SetPos(DirectX::XMFLOAT3(20.0f, 0.0f, -20.0f));
		soldiers[10].SetPos(DirectX::XMFLOAT3(-20.0f, 0.0f, 20.0f));
		soldiers[11].SetPos(DirectX::XMFLOAT3(-20.0f, 0.0f, -20.0f));

		soldiers[12].SetPos(DirectX::XMFLOAT3(10.0f, 0.0f, 10.0f));
		soldiers[13].SetPos(DirectX::XMFLOAT3(10.0f, 0.0f, -10.0f));
		soldiers[14].SetPos(DirectX::XMFLOAT3(-10.0f, 0.0f, 10.0f));
		soldiers[15].SetPos(DirectX::XMFLOAT3(-10.0f, 0.0f, -10.0f));

		soldiers[16].SetPos(DirectX::XMFLOAT3(40.0f, 20.0f, 40.0f));
		soldiers[17].SetPos(DirectX::XMFLOAT3(40.0f, 20.0f, -40.0f));
		soldiers[18].SetPos(DirectX::XMFLOAT3(-40.0f, 20.0f, 40.0f));
		soldiers[19].SetPos(DirectX::XMFLOAT3(-40.0f, 20.0f, -40.0f));

		soldiers[20].SetPos(DirectX::XMFLOAT3(30.0f, 20.0f, 30.0f));
		soldiers[21].SetPos(DirectX::XMFLOAT3(30.0f, 20.0f, -30.0f));
		soldiers[22].SetPos(DirectX::XMFLOAT3(-30.0f, 20.0f, 30.0f));
		soldiers[23].SetPos(DirectX::XMFLOAT3(-30.0f, 20.0f, -30.0f));

		soldiers[24].SetPos(DirectX::XMFLOAT3(40.0f, 0.0f, -40.0f));
		soldiers[25].SetPos(DirectX::XMFLOAT3(-40.0f, 0.0f, 40.0f));
		soldiers[26].SetPos(DirectX::XMFLOAT3(-40.0f, 0.0f, -40.0f));
	}


	ground.Init(texHandl, "../Resources/Obj/ground.obj", dir + "/VertexShader.cso", dir + "/HullShader.cso", dir + "/DomainShader.cso", dir + "/PixelShader.cso", dir + "/ComputeShader.cso", NO_SHADER, window.Gfx());
	ground.Scale(100.0f, 100.0f, 0.0f);
	ground.Rotate(DirectX::XMConvertToRadians(90), 0.0f, 0.0f);
	ground.Move(0.0f, -18.0f, 0.0f);
	grounds.push_back(&ground);

	ground1.Init(texHandl, "../Resources/Obj/ground.obj", dir + "/VertexShader.cso", dir + "/HullShader.cso", dir + "/DomainShader.cso", dir + "/PixelShader.cso", dir + "/ComputeShader.cso", NO_SHADER, window.Gfx());
	ground1.Scale(40.0f, 40.0f, 0.0f);
	ground1.Rotate(DirectX::XMConvertToRadians(90), 0.0f, 0.0f);
	ground1.Move(-150.0f, -20.0f, -50.0f);
	grounds.push_back(&ground1);

	ground2.Init(texHandl, "../Resources/Obj/ground.obj", dir + "/VertexShader.cso", dir + "/HullShader.cso", dir + "/DomainShader.cso", dir + "/PixelShader.cso", dir + "/ComputeShader.cso", NO_SHADER, window.Gfx());
	ground2.Scale(40.0f, 40.0f, 0.0f);
	ground2.Rotate(DirectX::XMConvertToRadians(90), 0.0f, 0.0f);
	ground2.Move(-150.0f, -20.0f, 0.0f);
	grounds.push_back(&ground2);

	ground3.Init(texHandl, "../Resources/Obj/ground.obj", dir + "/VertexShader.cso", dir + "/HullShader.cso", dir + "/DomainShader.cso", dir + "/PixelShader.cso", dir + "/ComputeShader.cso", NO_SHADER, window.Gfx());
	ground3.Scale(40.0f, 40.0f, 0.0f);
	ground3.Rotate(DirectX::XMConvertToRadians(90), 0.0f, 0.0f);
	ground3.Move(-150.0f, -20.0f, 50.0f);
	grounds.push_back(&ground3);

	cube.Init(texHandl, "../Resources/Obj/cubeTex.obj", dir + "/VertexShader.cso", dir + "/HullShader.cso", dir + "/DomainShader.cso", dir + "/PixelShader.cso", dir + "/ComputeShader.cso", NO_SHADER, window.Gfx());
	cube.Scale(2.0f, 2.0f, 2.0f);

	particle.Init(texHandl, "No", dir + "/VertexShaderParticle.cso", NO_SHADER, NO_SHADER, dir + "/PixelShaderParticle.cso", dir + "/ComputeShaderParticle.cso", dir + "/GeometryShaderParticle.cso", window.Gfx(), true);

}

void Scene::UpdateCamera()
{
	window.Gfx()->SetCamera(cam.GettViewMatrix());
}

void Scene::UpdateProjection()
{
	window.Gfx()->SetProjection(cam.GettProjectionMatrix());
	//window.Gfx()->SetProjection(proj);
}

void Scene::UpdateMouseDelta()
{
	if (window.mouse.GetPosX() > this->mouseXtemp || window.mouse.GetPosX() < this->mouseXtemp) {
		this->mouseDX = window.mouse.GetPosX() - this->mouseXtemp;
		this->mouseXtemp = (float)window.mouse.GetPosX();
	}
	if (window.mouse.GetPosY() > this->mouseYtemp || window.mouse.GetPosY() < this->mouseYtemp) {
		this->mouseDY = window.mouse.GetPosY() - this->mouseYtemp;
		this->mouseYtemp = (float)window.mouse.GetPosY();
	}
}