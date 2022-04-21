#include "Scene.h"

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
	:window(800, 600, L"Project"), dLight(DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f)), shadow(window.Gfx(), &dLight) ,cMap(window.Gfx()),
	soldier1(*window.Gfx()), soldier2(*window.Gfx()), soldier3(*window.Gfx()), soldier4(*window.Gfx()), soldier5(*window.Gfx()), soldier6(*window.Gfx()),
	cube(*window.Gfx())
{
	float fov = 90.0f; //90 degrees field of view
	float fovRadius = (fov / 360.0f) * DirectX::XM_2PI;//vertical field of view angle in radians
	float aspectRatio = static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight());//The aspect ratio
	float nearZ = 0.1f; //Minimum viewing 
	float farZ = 1000.0f;//Maximum viewing distance
	proj = DirectX::XMMatrixPerspectiveFovLH(fovRadius, aspectRatio, nearZ, farZ);
	window.Gfx()->SetProjection(proj);


	soldier1.Init("../Resources/Obj/elite.obj", "../Debug/VertexShader.cso", "../Debug/HullShader.cso", "../Debug/DomainShader.cso", "../Debug/PixelShader.cso", "../Debug/ComputeShader.cso", window.Gfx());
	soldier1.Move(0.0f, 10.0f, 0.0f);
	soldier1.Scale(2.0f, 2.0f, 2.0f);
	soldier2.Init("../Resources/Obj/elite.obj", "../Debug/VertexShader.cso", "../Debug/HullShader.cso", "../Debug/DomainShader.cso", "../Debug/PixelShader.cso", "../Debug/ComputeShader.cso", window.Gfx());
	soldier2.Move(0.0f, -10.0f, 0.0f);
	soldier2.Scale(2.0f, 2.0f, 2.0f);
	soldier3.Init("../Resources/Obj/elite.obj", "../Debug/VertexShader.cso", "../Debug/HullShader.cso", "../Debug/DomainShader.cso", "../Debug/PixelShader.cso", "../Debug/ComputeShader.cso", window.Gfx());
	soldier3.Move(10.0f, 0.0f, 0.0f);
	soldier3.Scale(2.0f, 2.0f, 2.0f);
	soldier4.Init("../Resources/Obj/elite.obj", "../Debug/VertexShader.cso", "../Debug/HullShader.cso", "../Debug/DomainShader.cso", "../Debug/PixelShader.cso", "../Debug/ComputeShader.cso", window.Gfx());
	soldier4.Move(-10.0f, 0.0f, 0.0f);
	soldier4.Scale(2.0f, 2.0f, 2.0f);
	soldier5.Init("../Resources/Obj/elite.obj", "../Debug/VertexShader.cso", "../Debug/HullShader.cso", "../Debug/DomainShader.cso", "../Debug/PixelShader.cso", "../Debug/ComputeShader.cso", window.Gfx());
	soldier5.Move(0.0f, 0.0f, 10.0f);
	soldier5.Scale(2.0f, 2.0f, 2.0f);
	soldier6.Init("../Resources/Obj/elite.obj", "../Debug/VertexShader.cso", "../Debug/HullShader.cso", "../Debug/DomainShader.cso", "../Debug/PixelShader.cso", "../Debug/ComputeShader.cso", window.Gfx());
	soldier6.Move(0.0f, 0.0f, -10.0f);
	soldier6.Scale(2.0f, 2.0f, 2.0f);

	cube.Init("../Resources/Obj/cubeTex.obj", "../Debug/VertexShader.cso", NO_SHADER,  NO_SHADER, "../Debug/PixelShaderCubeMap.cso", NO_SHADER, window.Gfx());
	cube.Move(0.0f, 1.0f, 0.0f);
	cube.Scale(2.0f, 2.0f, 2.0f);
	
	dLight.color = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
	dLight.direction = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
	SetUpDirLight();
	//cam.SetPos(DirectX::XMFLOAT3(0.0f, 10.0f, 0.0f));
	cam.SetDir(DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f));
	SetUpCamBuf();

	shadow.SetCamDir(*cam.GetDir());
	shadow.SetCamPos(*cam.GetPos());

}

Scene::~Scene()
{
	lightBuf->Release();
	camBuf->Release();

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
	timerCount += t;
	std::wstring timerString = L"Time elapsed " + std::to_wstring(timerCount);

	std::wstring dirStr = L"X: "+std::to_wstring(cam.GetDir()->x) + L", Y: " + std::to_wstring(cam.GetDir()->y) + L", Z: " + std::to_wstring(cam.GetDir()->z);

	window.SetTitle(dirStr.c_str());

	//Shadows
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

	//Cube mapping Start
	cMap.Clear(window.Gfx()->GetContext());
	//Go through all rtvs
	for (int i = 0; i < NUM_TEX; i++) {
		//Rotate camera
		
		cubeMapSetCam(i);
		
		//Render to current rtv
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

		shadow.BindDepthResource();
		window.Gfx()->GetContext()->HSSetConstantBuffers(0, 1, &camBuf);
		window.Gfx()->GetContext()->CSSetConstantBuffers(1, 1, &lightBuf);
		window.Gfx()->GetContext()->CSSetConstantBuffers(2, 1, &camBuf);
		window.Gfx()->EndFrame(W_H_CUBE, W_H_CUBE, CUBE_MAP);
	}

	cMap.SetSeccond(window.Gfx()->GetContext());
	window.Gfx()->EndFrame(window.GetWidth(), window.GetHeight(),CUBE_MAP_TWO);//Special case
	cube.Draw(window.Gfx(), CUBE_MAP_TWO);

	//Cube mapping end


	window.Gfx()->StartFrame(0.0f, 0.0f, 0.0f);


	window.Gfx()->SetProjection(proj);
	window.Gfx()->SetCamera(cam.GetMatrix());
	UpdateCam();
	checkInput();


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

	soldier1.Draw(window.Gfx());
	soldier2.Draw(window.Gfx());
	soldier3.Draw(window.Gfx());
	soldier4.Draw(window.Gfx());
	soldier5.Draw(window.Gfx());
	soldier6.Draw(window.Gfx());



	shadow.BindDepthResource();
	window.Gfx()->GetContext()->HSSetConstantBuffers(0, 1, &camBuf);
	window.Gfx()->GetContext()->CSSetConstantBuffers(1, 1, &lightBuf);
	window.Gfx()->GetContext()->CSSetConstantBuffers(2, 1, &camBuf);
	window.Gfx()->EndFrame(window.GetWidth(), window.GetHeight());

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

bool Scene::SetUpCamBuf()
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

	return !FAILED(hr);
}

void Scene::UpdateCam()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;//Create a mapped resource
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));//Clear the mappedResource
	HRESULT hr = window.Gfx()->GetContext()->Map(camBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);//Disable GPU access to the data
	CopyMemory(mappedResource.pData, cam.GetPos(), sizeof(DirectX::XMFLOAT3));//Write the new memory
	window.Gfx()->GetContext()->Unmap(camBuf, 0);//Reenable GPU access to the data
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
