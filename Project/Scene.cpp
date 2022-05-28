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
	:window(WIDTH, HEIGHT, L"Project"), dLight(DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f)), shadow(window.Gfx(), &dLight), cMap(window.Gfx())
{
	cam.SetPosition(0.0f, 0.0f, -3.0f),
	cam.SetProj(90.0f, window.GetWidth(), window.GetHeight(), 0.1f, 100.0f);
	window.Gfx()->SetProjection(cam.GettProjectionMatrix());
	window.Gfx()->SetCamera(cam.GettViewMatrix());


	SetUpGameObjects();


	dLight.color = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
	dLight.direction = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
	SetUpDirLight();
	//cam.SetPos(DirectX::XMFLOAT3(0.0f, 10.0f, 0.0f));
	//cam.SetDir(DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f));
	SetUpBufs();



	qtree = new QuadTree(gameObjects, 0, 100.0f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	tesselation = true;
	tesselationTemp = tesselation;
	quadTreeOn = true;
	frustumCheckOn = true;
	dt = 0;
	this->updateCulling = true;

	particle.Init(texHandl, "No", "../Debug/VertexShaderParticle.cso", NO_SHADER, NO_SHADER, "../Debug/PixelShaderParticle.cso", "../Debug/ComputeShaderParticle.cso", "../Debug/GeometryShaderParticle.cso", window.Gfx(), true);

	this->mouseXtemp = window.mouse.GetPosX();
	this->mouseYtemp = window.mouse.GetPosY();

	HandleCulling();

}

Scene::~Scene()
{
	if(lightBuf)lightBuf->Release();
	if(camBuf)camBuf->Release();
	if(camBuf2)camBuf2->Release();
	if (camBufTime)camBufTime->Release();

	texHandl->Delete();

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
		HandleCulling();
		this->updateCulling = false;
	}

	//Shadows Start First
	DirectX::XMFLOAT3 tempPos = cam.GetPositionFloat3();
	DirectX::XMFLOAT3 tempDir = cam.GetRotationFloat3();
	cam.SetPosition(0.0f, 20.0f, 0.0f);
	cam.SetRotationDeg(90, 0.0f , 0.0f);
	shadow.SetDirLight(&dLight);
	shadow.StartFirst(cam.GetPositionFloat3(), DIRECTIONAL_LIGHT);
	shadow.UpdateWhatShadow(0, DIRECTIONAL_LIGHT);
	UpdateCamera();
	window.Gfx()->StartFrame(0.0f, 0.0f, 0.0f, SHADOW);
	for (auto p : objectsToDraw) {
		p->Draw(window.Gfx(), SHADOW);
	}
	shadow.EndFirst();
	cam.SetPosition(tempPos);
	cam.SetRotationRad(tempDir);
	UpdateCamera();
	//Shadows End First
	//Shadows Start Second

	
	//Shadows End Second


	//Cube mapping first Start
	cMap.Clear(window.Gfx()->GetContext());
	//Go through all uavs
	for (int i = 0; i < NUM_TEX; i++) {
		//Rotate camera
		cubeMapSetCam(i);

		//Render to current uavs
		window.Gfx()->StartFrame(0.0f, 0.0f, 0.0f, CUBE_MAP);
		window.Gfx()->SetProjection(cMap.GetProj());/////////////////Cmaps cam has a projection, set that!
		window.Gfx()->SetCamera(cMap.GetCam().GettViewMatrix());

		cMap.Set(window.Gfx()->GetContext(), i);

		for (auto p : objectsToDraw) {
			p->Draw(window.Gfx(), CUBE_MAP);
		}
		for (int i = 0; i < 6; i++) {
			skybox[i].Draw(window.Gfx(), CUBE_MAP);
		}
		ground.Draw(window.Gfx(), CUBE_MAP);

		window.Gfx()->GetContext()->PSSetConstantBuffers(0, 1, &camBuf);
		window.Gfx()->GetContext()->HSSetConstantBuffers(0, 1, &camBuf);
		window.Gfx()->GetContext()->CSSetConstantBuffers(1, 1, &lightBuf);
		window.Gfx()->GetContext()->CSSetConstantBuffers(2, 1, &camBuf);
		window.Gfx()->EndFrame(W_H_CUBE, W_H_CUBE, CUBE_MAP);
	}
	//Cube map first end

	//Final draw Start
	window.Gfx()->StartFrame(0.0f, 0.0f, 0.0f);
	ImGuiWindows();
	UpdateProjection();
	UpdateCamera();
	UpdateBufs();
	checkInput();
	UpdateObjcects(t);

	//Cube map seccond Start
	cMap.SetSeccond(window.Gfx());
	cube.Draw(window.Gfx(), CUBE_MAP_TWO);
	cMap.SetEnd(window.Gfx());
	//Cube map seccond End

	for (auto p : objectsToDraw) {
		p->Draw(window.Gfx());
	}
	for (int i = 0; i < 6; i++) {
		skybox[i].Draw(window.Gfx());
	}
	ground.Draw(window.Gfx());

	//Particle Start
	window.Gfx()->GetContext()->GSSetConstantBuffers(0, 1, &camBuf);
	window.Gfx()->GetContext()->GSSetConstantBuffers(1, 1, &camBuf2);
	particle.Draw(window.Gfx(), PARTICLE);
	//Particle End

	window.Gfx()->GetContext()->HSSetConstantBuffers(0, 1, &camBuf);
	window.Gfx()->GetContext()->CSSetConstantBuffers(1, 1, &lightBuf);
	window.Gfx()->GetContext()->CSSetConstantBuffers(2, 1, &camBuf);
	window.Gfx()->EndFrame(window.GetWidth(), window.GetHeight());
	//Final draw End

	//Update particles
	particle.UpdateParticle(window.Gfx());
	window.Gfx()->GetContext()->CSSetConstantBuffers(3, 1, &camBufTime);
	window.Gfx()->UpdateParticles();

	return true;
}

bool Scene::SetUpDirLight()
{


	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = roundUpTo(sizeof(dLight), 16);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &dLight;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = window.Gfx()->GetDevice()->CreateBuffer(&desc, &data, &lightBuf);

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

	desc.ByteWidth = roundUpTo(sizeof(cam.GetForwardFloat3()), 16);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	data.pSysMem = &cam.GetForwardFloat3();
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	hr = window.Gfx()->GetDevice()->CreateBuffer(&desc, &data, &camBuf2);
	if (FAILED(hr)) {
		return false;
	}

	
	theTimedata.dt = dt;
	theTimedata.time = timerCount;
	desc.ByteWidth = roundUpTo(sizeof(TimeData), 16);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	data.pSysMem = &dt;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

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
		assert(false, "Failed to update buffer.");
	}

	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));//Clear the mappedResource
	hr = window.Gfx()->GetContext()->Map(camBuf2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);//Disable GPU access to the data
	CopyMemory(mappedResource.pData, &cam.GetForwardFloat3(), sizeof(DirectX::XMFLOAT3));//Write the new memory
	window.Gfx()->GetContext()->Unmap(camBuf2, 0);//Reenable GPU access to the data
	if (FAILED(hr)) {
		assert(false, "Failed to update buffer.");
	}

	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));//Clear the mappedResource
	hr = window.Gfx()->GetContext()->Map(camBufTime, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);//Disable GPU access to the data
	CopyMemory(mappedResource.pData, &theTimedata, sizeof(TimeData));//Write the new memory
	window.Gfx()->GetContext()->Unmap(camBufTime, 0);//Reenable GPU access to the data
	if (FAILED(hr)) {
		assert(false, "Failed to update buffer.");
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
	if (!ground.Update(0.0f, window.Gfx())) {
		std::cerr << "Failed to update object.\n";
		return false;
	}
	if (!cube.Update(0.0f, window.Gfx())) {
		std::cerr << "Failed to update object.\n";
		return false;
	}

	for (int i = 0; i < 6; i++) {
		if (!skybox[i].Update(0.0f, window.Gfx())) {
			std::cerr << "Failed to update object.\n";
			return false;
		}
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
		this->mouseXtemp = window.mouse.GetPosX();
		this->mouseYtemp = window.mouse.GetPosY();
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
	switch (num)
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
	}
}

void Scene::SetUpSkybox()
{
	skybox[0].Scale(100.0f, 100.0f, 0.0f);
	skybox[1].Scale(100.0f, 100.0f, 0.0f);
	skybox[2].Scale(100.0f, 100.0f, 0.0f);
	skybox[3].Scale(100.0f, 100.0f, 0.0f);
	skybox[4].Scale(100.0f, 100.0f, 0.0f);
	skybox[5].Scale(100.0f, 100.0f, 0.0f);

	skybox[0].Move(0.0f, 0.0f, 50.0f); //Front
	skybox[1].Move(0.0f, 0.0f, -50.0f); //Back
	skybox[2].Move(50.0f, 0.0f, 0.0f); //Right
	skybox[3].Move(-50.0f, 0.0f, 0.0f); //Left
	skybox[4].Move(0.0f, 50.0f, 0.0f); //Up
	skybox[5].Move(0.0f, -50.0f, 0.0f); //Down

	skybox[1].Rotate(0.0f, DegToRad(180.0f), 0.0f);
	skybox[2].Rotate(0.0f, DegToRad(90.0f), 0.0f);
	skybox[3].Rotate(0.0f, DegToRad(-90.0f), 0.0f);
	skybox[4].Rotate(DegToRad(-90.0f), 0.0f, 0.0f);
	skybox[5].Rotate(DegToRad(90.0f), 0.0f, 0.0f);
}

void Scene::ImGuiWindows()
{
	if (ImGui::Begin("Settings")) {
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::SliderFloat("Speed Factor", &speedfactor, 0.0f, 4.0f);
		ImGui::Checkbox("Tesselation", &tesselation);
		ImGui::Checkbox("QuadTree", &quadTreeOn);
		ImGui::Checkbox("Frustum Collision Check", &frustumCheckOn);

	}
	ImGui::End();
	if (ImGui::Begin("Player")) {
		ImGui::SliderFloat("Player Speed", &cam.GetSpeed(), 0.0f, 1.0f);
		ImGui::SliderFloat("Rotation Speed", &cam.GetRotSpeed(), 0.0f, 5.0f);
		ImGui::SliderFloat("Frustum buffer zone", cam.GetFrustumBuffer(), 0.0, 20.0f);
	}
	ImGui::End();
}

float Scene::DegToRad(float deg)
{
	double pi = 3.14159265359;
	return (deg * (pi / 180));
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

void Scene::HandleCulling()
{
	if (quadTreeOn) {
		intersectingNodes.clear();
		for (auto p : objectsToDraw) {
			p->Remove();
		}
		objectsToDraw.clear();
		qtree->InsideNodes(cam, &intersectingNodes);
		std::vector<SceneObjectTest*> objectsTemp;
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
	else{
		objectsToDraw.clear();
		for (auto p : gameObjects) {
			objectsToDraw.push_back(p);
		}
	}
}

void Scene::SetUpGameObjects()
{
	for (int i = 0; i < NUM_SOLDIERS; i++) {
		soldiers[i].Init(texHandl, "../Resources/Obj/elite.obj", "../Debug/VertexShader.cso", "../Debug/HullShader.cso", "../Debug/DomainShader.cso", "../Debug/PixelShader.cso", "../Debug/ComputeShader.cso", NO_SHADER, window.Gfx());
		soldiers[i].Scale(2.0f, 2.0f, 2.0f);
		gameObjects.push_back(&soldiers[i]);
	}
	soldiers[0].SetPos(DirectX::XMFLOAT3(40.0f, 0.0f, 40.0f));
	soldiers[1].SetPos(DirectX::XMFLOAT3(40.0f, 0.0f, -40.0f));
	soldiers[2].SetPos(DirectX::XMFLOAT3(-40.0f, 0.0f, 40.0f));
	soldiers[3].SetPos(DirectX::XMFLOAT3(-40.0f, 0.0f, -40.0f));

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

	ground.Init(texHandl, "../Resources/Obj/ground.obj", "../Debug/VertexShader.cso", "../Debug/HullShader.cso", "../Debug/DomainShader.cso", "../Debug/PixelShader.cso", "../Debug/ComputeShader.cso", NO_SHADER, window.Gfx());
	ground.Scale(100.0f, 100.0f, 0.0f);
	ground.Rotate(DegToRad(90), 0.0f, 0.0f);
	ground.Move(0.0f, -20.0f, 0.0f);
	gameObjects.push_back(&ground);
	

	cube.Init(texHandl, "../Resources/Obj/cubeTex.obj", "../Debug/VertexShader.cso", "../Debug/HullShader.cso", "../Debug/DomainShader.cso", "../Debug/PixelShader.cso", "../Debug/ComputeShader.cso", NO_SHADER, window.Gfx());
	cube.Scale(2.0f, 2.0f, 2.0f);

	for (int i = 0; i < 6; i++) {
		skybox[i].Init(texHandl, "../Resources/Obj/plane.obj", "../Debug/VertexShader.cso", "../Debug/HullShader.cso", "../Debug/DomainShader.cso", "../Debug/PixelShader.cso", "../Debug/ComputeShader.cso", NO_SHADER, window.Gfx());
	}
	SetUpSkybox();
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
		this->mouseXtemp = window.mouse.GetPosX();
	}
	if (window.mouse.GetPosY() > this->mouseYtemp || window.mouse.GetPosY() < this->mouseYtemp) {
		this->mouseDY = window.mouse.GetPosY() - this->mouseYtemp;
		this->mouseYtemp = window.mouse.GetPosY();
	}
}

