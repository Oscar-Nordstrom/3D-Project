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
	:window(800, 600, L"Project"), object(*window.Gfx()), floor(*window.Gfx()), dLight(DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f)), shadow(window.Gfx(), &dLight), test(*window.Gfx())//, skybox(*window.Gfx())
{
	float fov = 90.0f; //90 degrees field of view
	float fovRadius = (fov / 360.0f) * DirectX::XM_2PI;//vertical field of view angle in radians
	float aspectRatio = static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight());//The aspect ratio
	float nearZ = 0.1f; //Minimum viewing 
	float farZ = 1000.0f;//Maximum viewing distance
	proj = DirectX::XMMatrixPerspectiveFovLH(fovRadius, aspectRatio, nearZ, farZ);
	window.Gfx()->SetProjection(proj);

	test.Init("../Resources/Obj/theo_the_teddybear.obj", "../Debug/VertexShader.cso","../Debug/HullShader.cso", "../Debug/DomainShader.cso", "../Debug/PixelShader.cso", "../Debug/ComputeShader.cso", window.Gfx());
	test.Move(-2.0f, -3.0f, 0.0f);
	test.Rotate(0.0f, 3.0, 0.0f);
	test.Scale(-0.95f, -0.95f, -0.95f);
	object.Init("../Resources/Obj/elite.obj", "../Debug/VertexShader.cso", "../Debug/HullShader.cso", "../Debug/DomainShader.cso", "../Debug/PixelShader.cso", "../Debug/ComputeShader.cso", window.Gfx());
	object.Move(2.0f, 0.0f, 0.0f);
	object.Scale(2.0f, 2.0f, 2.0f);
	floor.Init("../Resources/Obj/cubeTex.obj", "../Debug/VertexShader.cso", "../Debug/HullShader.cso", "../Debug/DomainShader.cso", "../Debug/PixelShader.cso", "../Debug/ComputeShader.cso", window.Gfx());
	floor.Move(0.0f, -10.0f, 0.0f);
	floor.Scale(10.0f, -0.9f, 10.0f);
	//floor.Rotate(0.0f, 0.0f, 0.0f);
	//skybox.Init("../Resources/Obj/skybox.obj", "../Debug/VertexShader.cso", "../Debug/HullShader.cso", "../Debug/DomainShader.cso", "../Debug/PixelShader.cso", "../Debug/ComputeShader.cso", window.Gfx());
	//skybox.Scale(-200.0f, -200.0f, -200.0f);
	dLight.color = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
	dLight.direction = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
	SetUpDirLight();
	//cam.SetPos(DirectX::XMFLOAT3(0.0f, 10.0f, 0.0f));
	cam.SetDir(DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f));
	SetUpCamBuf();

	shadow.SetCamDir(*cam.GetDir());
	shadow.SetCamPos(*cam.GetPos());

	//sponza.Load("../Resources/Obj/sponza.obj", "bla", "bla", "bla", window.Gfx());
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

	test.Draw(window.Gfx(), false);
	object.Draw(window.Gfx(), false);
	floor.Draw(window.Gfx(), false);
	//skybox.Draw(window.Gfx(), false);
	

	window.Gfx()->StartFrame(0.0f, 0.0f, 0.0f);


	window.Gfx()->SetProjection(proj);
	window.Gfx()->SetCamera(cam.GetMatrix());
	UpdateCam();
	checkInput();

	if (!test.Update(0.0f, window.Gfx())) {
		std::cerr << "Failed to update test object.\n";
		return false;
	}
	if (!object.Update(t, window.Gfx())) {
		std::cerr << "Failed to update object.\n";
		return false;
	}
	if (!floor.Update(0.0f, window.Gfx())) {
		std::cerr << "Failed to update object.\n";
		return false;
	}
	/*if (!skybox.Update(0.0f, window.Gfx())) {
		std::cerr << "Failed to update object.\n";
		return false;
	}*/
	object.Draw(window.Gfx());
	floor.Draw(window.Gfx());
	test.Draw(window.Gfx());
	//skybox.Draw(window.Gfx());


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
	DirectX::XMFLOAT3 move = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	if (window.Kbd()->KeyIsPressed('W')) {
		move.z = 1;
	}
	else if (window.Kbd()->KeyIsPressed('S')) {
		move.z = -1;
	}
	if (window.Kbd()->KeyIsPressed('D')) {
		move.x = 1;
	}
	else if (window.Kbd()->KeyIsPressed('A')) {
		move.x = -1;
	}
	if (window.Kbd()->KeyIsPressed('U')) {
		move.y = 1;
	}
	else if (window.Kbd()->KeyIsPressed('N')) {
		move.y = -1;
	}
	cam.Move(move);
}
