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
	:window(WIDTH, HEIGHT, L"Project"), dLight(DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f)), shadow(window.Gfx(), &dLight), cMap(window.Gfx()),
	soldier1(*window.Gfx(), texHandl), soldier2(*window.Gfx(), texHandl), soldier3(*window.Gfx(), texHandl), soldier4(*window.Gfx(), texHandl), 
	soldier5(*window.Gfx(), texHandl), soldier6(*window.Gfx(), texHandl),
	cube(*window.Gfx(), texHandl), particle(*window.gfx, texHandl)
{
	float fov = 90.0f; //90 degrees field of view
	float fovRadius = (fov / 360.0f) * DirectX::XM_2PI;//vertical field of view angle in radians
	float aspectRatio = static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight());//The aspect ratio
	float nearZ = 0.1f; //Minimum viewing 
	float farZ = 1000.0f;//Maximum viewing distance
	proj = DirectX::XMMatrixPerspectiveFovLH(fovRadius, aspectRatio, nearZ, farZ);
	window.Gfx()->SetProjection(proj);

	//ground.Init("../Resources/Obj/ground.obj", "../Debug/VertexShader.cso", "../Debug/HullShader.cso", "../Debug/DomainShader.cso", "../Debug/PixelShader.cso", "../Debug/ComputeShader.cso", NO_SHADER, window.Gfx());
	//ground.Scale(200.0f, 200.0f, 0.0f);
	//ground.Rotate(DegToRad(90.0f), 0.0f, 0.0f);
	//ground.Move(0.0f, -5.0f, 0.0f);

	soldier1.Init("../Resources/Obj/elite.obj", "../Debug/VertexShader.cso", "../Debug/HullShader.cso", "../Debug/DomainShader.cso", "../Debug/PixelShader.cso", "../Debug/ComputeShader.cso", NO_SHADER, window.Gfx());
	soldier1.Move(0.0f, 10.0f, 0.0f);
	soldier1.Scale(2.0f, 2.0f, 2.0f);
	soldier2.Init("../Resources/Obj/elite.obj", "../Debug/VertexShader.cso", "../Debug/HullShader.cso", "../Debug/DomainShader.cso", "../Debug/PixelShader.cso", "../Debug/ComputeShader.cso", NO_SHADER, window.Gfx());
	soldier2.Move(0.0f, 20.0f, 0.0f);
	soldier2.Scale(2.0f, 2.0f, 2.0f);
	soldier3.Init("../Resources/Obj/elite.obj", "../Debug/VertexShader.cso", "../Debug/HullShader.cso", "../Debug/DomainShader.cso", "../Debug/PixelShader.cso", "../Debug/ComputeShader.cso", NO_SHADER, window.Gfx());
	soldier3.Move(10.0f, 0.0f, 0.0f);
	soldier3.Scale(2.0f, 2.0f, 2.0f);
	soldier4.Init("../Resources/Obj/elite.obj", "../Debug/VertexShader.cso", "../Debug/HullShader.cso", "../Debug/DomainShader.cso", "../Debug/PixelShader.cso", "../Debug/ComputeShader.cso", NO_SHADER, window.Gfx());
	soldier4.Move(-10.0f, 0.0f, 0.0f);
	soldier4.Scale(2.0f, 2.0f, 2.0f);
	soldier5.Init("../Resources/Obj/elite.obj", "../Debug/VertexShader.cso", "../Debug/HullShader.cso", "../Debug/DomainShader.cso", "../Debug/PixelShader.cso", "../Debug/ComputeShader.cso", NO_SHADER, window.Gfx());
	soldier5.Move(0.0f, 0.0f, 10.0f);
	soldier5.Scale(2.0f, 2.0f, 2.0f);
	soldier6.Init("../Resources/Obj/elite.obj", "../Debug/VertexShader.cso", "../Debug/HullShader.cso", "../Debug/DomainShader.cso", "../Debug/PixelShader.cso", "../Debug/ComputeShader.cso", NO_SHADER, window.Gfx());
	soldier6.Move(0.0f, 0.0f, -10.0f);
	soldier6.Scale(2.0f, 2.0f, 2.0f);

	cube.Init("../Resources/Obj/cubeTex.obj", "../Debug/VertexShader.cso", "../Debug/HullShader.cso", "../Debug/DomainShader.cso", "../Debug/PixelShader.cso", "../Debug/ComputeShader.cso", NO_SHADER, window.Gfx());
	cube.Scale(2.0f, 2.0f, 2.0f);


	for (int i = 0; i < 6; i++) {
		skybox.push_back(new SceneObjectTest(*window.Gfx(), texHandl));
		skybox[i]->Init("../Resources/Obj/plane.obj", "../Debug/VertexShader.cso", "../Debug/HullShader.cso", "../Debug/DomainShader.cso", "../Debug/PixelShader.cso", "../Debug/ComputeShader.cso",NO_SHADER, window.Gfx());
	}
	SetUpSkybox();

	dLight.color = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
	dLight.direction = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
	SetUpDirLight();
	//cam.SetPos(DirectX::XMFLOAT3(0.0f, 10.0f, 0.0f));
	cam.SetDir(DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f));
	SetUpBufs();

	shadow.SetCamDir(*cam.GetDir());
	shadow.SetCamPos(*cam.GetPos());

	gameObjects.push_back(&soldier1);
	gameObjects.push_back(&soldier2);
	gameObjects.push_back(&soldier3);
	gameObjects.push_back(&soldier4);
	gameObjects.push_back(&soldier5);
	gameObjects.push_back(&soldier6);
	gameObjects.push_back(&cube);

	qtree = new QuadTree(gameObjects, 0, 100.0f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	tesselation = true;
	tesselationTemp = tesselation;
	dt = 0;

	particle.Init("No", "../Debug/VertexShaderParticle.cso", NO_SHADER, NO_SHADER, "../Debug/PixelShaderParticle.cso", "../Debug/ComputeShaderParticle.cso", "../Debug/GeometryShaderParticle.cso", window.Gfx(), true);
}

Scene::~Scene()
{
	if(lightBuf)lightBuf->Release();
	if(camBuf)camBuf->Release();
	if(camBuf2)camBuf2->Release();
	if (camBufTime)camBufTime->Release();

	for (int i = 0; i < 6; i++) {
		if (skybox[i] != nullptr)
			delete skybox[i];
	}

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
	
	std::wstring dirStr = L"X: " + std::to_wstring(cam.GetPos()->x) + L", Y: " + std::to_wstring(cam.GetPos()->y) + L", Z: " + std::to_wstring(cam.GetPos()->z);

	window.SetTitle(dirStr.c_str());

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


	//Shadows Start
	shadow.SetCamPos(DirectX::XMFLOAT3(0.0f, 10.0f, 0.0f));
	shadow.SetCamDir(DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f));
	//Using the same camera
	shadow.SetShadowMap();
	soldier1.Draw(window.Gfx(), SHADOW);
	soldier2.Draw(window.Gfx(), SHADOW);
	soldier3.Draw(window.Gfx(), SHADOW);
	soldier4.Draw(window.Gfx(), SHADOW);
	soldier5.Draw(window.Gfx(), SHADOW);
	soldier6.Draw(window.Gfx(), SHADOW);

	window.Gfx()->SetProjection(proj);
	window.Gfx()->SetCamera(cam.GetMatrix());
	//Shadows End


	//Cube mapping first Start
	cMap.Clear(window.Gfx()->GetContext());
	//Go through all uavs
	for (int i = 0; i < NUM_TEX; i++) {
		//Rotate camera
		cubeMapSetCam(i);

		//Render to current uavs
		window.Gfx()->StartFrame(0.0f, 0.0f, 0.0f, CUBE_MAP);
		window.Gfx()->SetProjection(cMap.GetProj());
		window.Gfx()->SetCamera(cMap.GetCam().GetMatrix());

		cMap.Set(window.Gfx()->GetContext(), i);


		soldier1.Draw(window.Gfx(), CUBE_MAP);
		soldier2.Draw(window.Gfx(), CUBE_MAP);
		soldier3.Draw(window.Gfx(), CUBE_MAP);
		soldier4.Draw(window.Gfx(), CUBE_MAP);
		soldier5.Draw(window.Gfx(), CUBE_MAP);
		soldier6.Draw(window.Gfx(), CUBE_MAP);
		for (int i = 0; i < 6; i++) {
			skybox[i]->Draw(window.Gfx(), CUBE_MAP);
		}


		shadow.BindDepthResource();
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
	window.Gfx()->SetProjection(proj);
	window.Gfx()->SetCamera(cam.GetMatrix());
	UpdateBufs();
	checkInput();
	UpdateObjcects(t);

	//Cube map seccond Start
	cMap.SetSeccond(window.Gfx());
	cube.Draw(window.Gfx(), CUBE_MAP_TWO);
	cMap.SetEnd(window.Gfx());
	//Cube map seccond End
	soldier1.Draw(window.Gfx());
	soldier2.Draw(window.Gfx());
	soldier3.Draw(window.Gfx());
	soldier4.Draw(window.Gfx());
	soldier5.Draw(window.Gfx());
	soldier6.Draw(window.Gfx());
	//ground.Draw(window.Gfx());

	//Particle Start
	window.Gfx()->GetContext()->GSSetConstantBuffers(0, 1, &camBuf);
	window.Gfx()->GetContext()->GSSetConstantBuffers(1, 1, &camBuf2);
	particle.Draw(window.Gfx(), PARTICLE);
	//Particle End

	shadow.BindDepthResource();
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
	desc.ByteWidth = roundUpTo(sizeof(cam.GetPos()), 16);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = cam.GetPos();
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = window.Gfx()->GetDevice()->CreateBuffer(&desc, &data, &camBuf);
	if (FAILED(hr)) {
		return false;
	}

	desc.ByteWidth = roundUpTo(sizeof(cam.GetDir()), 16);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	data.pSysMem = cam.GetDir();
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
	CopyMemory(mappedResource.pData, cam.GetPos(), sizeof(DirectX::XMFLOAT3));//Write the new memory
	window.Gfx()->GetContext()->Unmap(camBuf, 0);//Reenable GPU access to the data
	if (FAILED(hr)) {
		assert(false, "Failed to update buffer.");
	}

	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));//Clear the mappedResource
	hr = window.Gfx()->GetContext()->Map(camBuf2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);//Disable GPU access to the data
	CopyMemory(mappedResource.pData, cam.GetDir(), sizeof(DirectX::XMFLOAT3));//Write the new memory
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
	if (!soldier1.Update(t, window.Gfx())) {
		std::cerr << "Failed to update object.\n";
		return false;
	}
	if (!soldier2.Update(t, window.Gfx())) {
		std::cerr << "Failed to update object.\n";
		return false;
	}
	if (!soldier3.Update(t, window.Gfx())) {
		std::cerr << "Failed to update object.\n";
		return false;
	}
	if (!soldier4.Update(t, window.Gfx())) {
		std::cerr << "Failed to update object.\n";
		return false;
	}
	if (!soldier5.Update(t, window.Gfx())) {
		std::cerr << "Failed to update object.\n";
		return false;
	}
	if (!soldier6.Update(t, window.Gfx())) {
		std::cerr << "Failed to update object.\n";
		return false;
	}

	if (!cube.Update(0.0f, window.Gfx())) {
		std::cerr << "Failed to update object.\n";
		return false;
	}

	for (int i = 0; i < 6; i++) {
		if (!skybox[i]->Update(0.0f, window.Gfx())) {
			std::cerr << "Failed to update object.\n";
			return false;
		}
	}
	for (int i = 0; i < 6; i++) {
		skybox[i]->Draw(window.Gfx());
	}

	/*if (!ground.Update(0.0f, window.Gfx())) {
		std::cerr << "Failed to update object.\n";
		return false;
	}*/

	return true;
}

void Scene::checkInput()
{
	if (window.Kbd()->KeyIsPressed('W')) {
		cam.forward();
	}
	else if (window.Kbd()->KeyIsPressed('S')) {
		cam.backward();
	}
	if (window.Kbd()->KeyIsPressed('D')) {
		cam.right();
	}
	else if (window.Kbd()->KeyIsPressed('A')) {
		cam.left();
	}
	//Rotate
	if (window.Kbd()->KeyIsPressed(LEFT_ARROW)) {
		cam.Rotate(-0.05f);
	}
	else if (window.Kbd()->KeyIsPressed(RIGHT_ARROW)) {
		cam.Rotate(0.05f);
	}
	//Up/down
	if (window.Kbd()->KeyIsPressed(SPACE)) {
		cam.up();
	}
	else if (window.Kbd()->KeyIsPressed(CTRL)) {
		cam.down();
	}
	//cam.Move(move);
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
		cMap.GetCam().SetDir(DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f));
		cMap.GetCam().SetUpDir(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
		break;
	case 1:
		cMap.GetCam().SetDir(DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f));
		cMap.GetCam().SetUpDir(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
		break;
	case 2:
		cMap.GetCam().SetDir(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
		cMap.GetCam().SetUpDir(DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f));
		break;
	case 3:
		cMap.GetCam().SetDir(DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f));
		cMap.GetCam().SetUpDir(DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f));
		break;
	case 4:
		cMap.GetCam().SetDir(DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f));
		cMap.GetCam().SetUpDir(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
		break;
	case 5:
		cMap.GetCam().SetDir(DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f));
		cMap.GetCam().SetUpDir(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
		break;
	}
}

void Scene::SetUpSkybox()
{
	skybox[0]->Scale(100.0f, 100.0f, 0.0f);
	skybox[1]->Scale(100.0f, 100.0f, 0.0f);
	skybox[2]->Scale(100.0f, 100.0f, 0.0f);
	skybox[3]->Scale(100.0f, 100.0f, 0.0f);
	skybox[4]->Scale(100.0f, 100.0f, 0.0f);
	skybox[5]->Scale(100.0f, 100.0f, 0.0f);

	skybox[0]->Move(0.0f, 0.0f, 50.0f); //Front
	skybox[1]->Move(0.0f, 0.0f, -50.0f); //Back
	skybox[2]->Move(50.0f, 0.0f, 0.0f); //Right
	skybox[3]->Move(-50.0f, 0.0f, 0.0f); //Left
	skybox[4]->Move(0.0f, 50.0f, 0.0f); //Up
	skybox[5]->Move(0.0f, -50.0f, 0.0f); //Down

	skybox[1]->Rotate(0.0f, DegToRad(180.0f), 0.0f);
	skybox[2]->Rotate(0.0f, DegToRad(90.0f), 0.0f);
	skybox[3]->Rotate(0.0f, DegToRad(-90.0f), 0.0f);
	skybox[4]->Rotate(DegToRad(-90.0f), 0.0f, 0.0f);
	skybox[5]->Rotate(DegToRad(90.0f), 0.0f, 0.0f);
}

void Scene::ImGuiWindows()
{
	if (ImGui::Begin("Settings")) {
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::SliderFloat("Speed Factor", &speedfactor, 0.0f, 4.0f);
		ImGui::Checkbox("Tesselation", &tesselation);

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

