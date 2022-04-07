#include "SceneObject.h"
#include "Graphics.h"

namespace dx = DirectX;

SceneObject::SceneObject(Graphics& gfx)
{
	indexBuffer = nullptr;
	inputLayout = nullptr;
	pShader = nullptr;
	vShader = nullptr;
	cShader = nullptr;
	samState = nullptr;
	vertexBuffer = nullptr;
	constantBuffer = nullptr;
	mtlFileTex = nullptr;
	topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	transform = dx::XMMatrixScaling(sx, sy, sz) * dx::XMMatrixRotationX(rx) * dx::XMMatrixRotationY(ry) * dx::XMMatrixRotationZ(rz) * dx::XMMatrixTranslation(x, y, z);
}

SceneObject::~SceneObject()
{
	if (vShader)vShader->Release();
	if (pShader)pShader->Release();
	if (cShader)cShader->Release();
	if (inputLayout)inputLayout->Release();
	if (samState)samState->Release();
	if (vertexBuffer)vertexBuffer->Release();
	if (indexBuffer)indexBuffer->Release();
	if (constantBuffer)constantBuffer->Release();

	for (auto o : subs) {
		o.Terminate();
	}



	delete mtlFileTex;

}

bool SceneObject::Update(float dt, Graphics& gfx)
{
	UpdateTransform(dt);
	if (!UpdateCbuf(gfx)) {
		std::cerr << "Failed to update constant buffer.\n";
		return false;
	}
	return true;
}

void SceneObject::Move(float dx, float dy, float dz)
{
	x += dx;
	y += dy;
	z += dz;
}

void SceneObject::Scale(float dx, float dy, float dz)
{

	sx += dx;
	sy += dy;
	sz += dz;
}

void SceneObject::Rotate(float dx, float dy, float dz)
{
	rx += dx;
	ry += dy;
	rz += dz;
}

void SceneObject::Draw(Graphics*& gfx, bool withShaders)
{
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	if (withShaders) {
		gfx->GetContext()->VSSetShader(vShader, nullptr, 0);
		gfx->GetContext()->PSSetShader(pShader, nullptr, 0);
		gfx->GetContext()->CSSetShader(cShader, nullptr, 0);
	}
	gfx->GetContext()->IASetInputLayout(inputLayout);
	if (withShaders) {
		gfx->GetContext()->PSSetSamplers(0, 1, &samState);
	}
	else {
		UpdateCbuf(*gfx);
	}
	gfx->GetContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	gfx->GetContext()->IASetPrimitiveTopology(topology);
	gfx->GetContext()->VSSetConstantBuffers(0, 1, &constantBuffer);



	if (subs.size() > 0) {
		for (auto& o : subs) {
			o.Bind(gfx->GetContext(), withShaders);
		}
	}
	else {

		gfx->GetContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

		gfx->GetContext()->DrawIndexed((UINT)indices.size(), 0, 0);
	}

}


bool SceneObject::Init(string meshPath, string vShaderPath, string pShaderPath, string cShaderPath, Graphics& gfx)
{
	if (!LoadMesh(gfx.GetDevice(), gfx.GetContext(), meshPath)) {
		std::cerr << "Failed to load mesh.\n";
		return false;
	}
	if (!LoadShaders(vShaderPath, pShaderPath, cShaderPath, gfx.GetDevice())) {
		std::cerr << "Failed to load shaders.\n";
		return false;
	}
	if (!CreateInputLayout(gfx.GetDevice())) {
		std::cerr << "Failed to create input layout.\n";
		return false;
	}
	if (!SetUpSampler(gfx.GetDevice())) {
		std::cerr << "Failed to create sampler.\n";
		return false;
	}
	if (!CreateVertexBuffer(gfx.GetDevice())) {
		std::cerr << "Failed to create vertex buffer.\n";
		return false;
	}
	if (!CreateIndexBuffer(gfx.GetDevice())) {
		std::cerr << "Failed to create index buffer.\n";
		return false;
	}
	if (!CreateConstantBuffer(gfx)) {
		std::cerr << "Failed to create constant buffer.\n";
		return false;
	}

	return true;
}

bool SceneObject::LoadShaders(string vShaderPath, string pShaderPath, string cShaderPath, ID3D11Device* const& device)
{
	std::string shaderData;
	std::ifstream reader;
	//Open the vertex shader cso file
	reader.open(vShaderPath, std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open VS file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);//Go to the end of the file
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));//Reserve space based on how big the file is
	reader.seekg(0, std::ios::beg);//Go the the start of the file

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>()); //Assign the file to the shaderdata

	//Create our vertwx shader and store it in vShader
	if (FAILED(device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &vShader)))
	{
		std::cerr << "Failed to create vertex shader!" << std::endl;
		return false;
	}

	//Give the shader the data
	vShaderByteCode = shaderData;
	shaderData.clear();//Clear the string with data
	reader.close();//Close the file
	//Open the pixel shader cso file
	reader.open(pShaderPath, std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open PS file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);//Go to the end of the file
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));//Reserve space based on the size of the file
	reader.seekg(0, std::ios::beg);//Go to the start of the file

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());//Assign the file to the shader data

	//Create the pixel shader and store it in pShader
	if (FAILED(device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pShader)))
	{
		std::cerr << "Failed to create pixel shader!" << std::endl;
		return false;
	}

	shaderData.clear();//Clear the string with data
	reader.close();//Close the file
	//Open the compute shader cso file
	reader.open(cShaderPath, std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open CS file!" << std::endl;
		return false;
	}
	reader.seekg(0, std::ios::end);//Go to the end of the file
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));//Reserve space based on the size of the file
	reader.seekg(0, std::ios::beg);//Go to the start of the file

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());//Assign the file to the shader data

	//Create the compute shader and store it in cShader
	if (FAILED(device->CreateComputeShader(shaderData.c_str(), shaderData.length(), nullptr, &cShader)))
	{
		std::cerr << "Failed to create pixel shader!" << std::endl;
		return false;
	}

	return true;
}

bool SceneObject::LoadMesh(ID3D11Device* device, ID3D11DeviceContext* context, string meshPath)
{
	int numF = 0;
	string mtlFile = "";

	bool submesh = false;
	bool useSubs = false;
	string mtl = "";
	string subName;
	int submeshStart = 0;
	int submeshEnd = 0;

	stringstream ss;
	ifstream file;
	string line;
	string prefix = "";
	DirectX::XMFLOAT3 temp3;
	DirectX::XMFLOAT2 temp2;
	DirectX::XMFLOAT3 tempV;
	DirectX::XMFLOAT3 tempVN;
	DirectX::XMFLOAT2 tempVT;
	int tempI = 0;
	file.open(meshPath);
	if (!file.is_open()) {
		std::cerr << "Failed to open mesh file.\n";
		return false;
	}
	while (getline(file, line)) {


		ss.clear();
		if (line[line.size() - 1] != ' ') {
			line += " ";
		}
		ss.str(line);
		ss >> prefix;
		if (prefix == "v") {
			ss >> temp3.x >> temp3.y >> temp3.z;
			v.push_back(temp3);
		}
		else if (prefix == "vt") {
			ss >> temp2.x >> temp2.y;
			vt.push_back(temp2);
		}
		else if (prefix == "vn") {
			ss >> temp3.x >> temp3.y >> temp3.z;
			vn.push_back(temp3);
		}
		else if (prefix == "f") {

			int c = 0;
			while (ss >> tempI) {
				if (c == 0) {
					tempV = v[tempI - 1];
				}
				else if (c == 1) {
					tempVT = vt[tempI - 1];
				}
				else if (c == 2) {
					tempVN = vn[tempI - 1];
				}
				if (ss.peek() == '/' || ss.peek() == ' ') {
					c++;
					ss.ignore(1, '/');
				}
				if (c > 2) {
					c = 0;
					SimpleVertex vertTemp(tempV, tempVN, tempVT);
					int tempIndex = GetIndex(vertTemp);
					if (tempIndex == -1) {
						verts.push_back(vertTemp);
						int toPush = verts.size() - 1;
						indices.push_back(toPush);
					}
					else {
						indices.push_back(tempIndex);
					}
					if (submesh) {
						submesh = false;
						submeshStart = indices.size() - 1;
						useSubs = true;
					}

				}

			}
		}
		else if (prefix == "mtllib") {
			ss >> mtlFile;
			//Read the mtl file and load in all the image files;
			mtlFileTex = new MtlImages(mtlFile, device);
			int idsds = 0;
		}
		else if (prefix == "g") {
			if (subs.size() > 0 || useSubs) {
				submeshEnd = indices.size() - 1;
				SubMesh* subM = new SubMesh(device, context, indices, mtlFileTex, subName, mtlFile, mtl, submeshStart, submeshEnd);
				subs.push_back(*subM);
			}
			submesh = true;
			ss >> subName;

		}
		else if (prefix == "usemtl") {
			ss >> mtl;
		}
	}
	if (subs.size() > 0) {
		submeshEnd = indices.size() - 1;
		SubMesh* subM = new SubMesh(device, context, indices, mtlFileTex, subName, mtlFile, mtl, submeshStart, submeshEnd);
		subs.push_back(*subM);
	}
	return true;
}

bool SceneObject::CreateInputLayout(ID3D11Device* device)
{
	D3D11_INPUT_ELEMENT_DESC inputDesc[3] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0,  24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT hr = device->CreateInputLayout(inputDesc, 3, vShaderByteCode.c_str(), vShaderByteCode.length(), &inputLayout);

	return !FAILED(hr);
}

bool SceneObject::SetUpSampler(ID3D11Device* device)
{
	D3D11_SAMPLER_DESC sampler;
	sampler.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 1;

	HRESULT hr = device->CreateSamplerState(&sampler, &samState);

	return !FAILED(hr);
}

bool SceneObject::CreateVertexBuffer(ID3D11Device* device)
{
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.ByteWidth = UINT(verts.size() * sizeof(SimpleVertex));
	bufferDesc.StructureByteStride = sizeof(SimpleVertex);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = verts.data();
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &vertexBuffer);

	return !FAILED(hr);
}

bool SceneObject::CreateIndexBuffer(ID3D11Device* device)
{
	D3D11_BUFFER_DESC ibDesc;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;
	ibDesc.ByteWidth = UINT(indices.size() * sizeof(unsigned short));
	ibDesc.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = indices.data();
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&ibDesc, &data, &indexBuffer);

	return !FAILED(hr);
}

bool SceneObject::CreateConstantBuffer(Graphics& gfx)
{
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.ByteWidth = sizeof(Matrices);
	cbDesc.StructureByteStride = 0;


	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 proj;
	DirectX::XMStoreFloat4x4(&world, transform);
	DirectX::XMStoreFloat4x4(&view, gfx.GetCamera());
	DirectX::XMStoreFloat4x4(&proj, gfx.GetProjection());
	const Matrices m(world, view, proj);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &m;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = gfx.GetDevice()->CreateBuffer(&cbDesc, &data, &constantBuffer);

	return !FAILED(hr);
}

bool SceneObject::UpdateCbuf(Graphics& gfx)
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 proj;
	DirectX::XMStoreFloat4x4(&world, transform);
	DirectX::XMStoreFloat4x4(&view, gfx.GetCamera());
	DirectX::XMStoreFloat4x4(&proj, gfx.GetProjection());
	const Matrices m(world, view, proj);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));//Clear the mappedResource
	HRESULT hr = gfx.GetContext()->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &m, sizeof(Matrices));//Write the new memory
	gfx.GetContext()->Unmap(constantBuffer, 0);

	return !FAILED(hr);
}

int SceneObject::GetIndex(SimpleVertex v)
{

	for (unsigned int i = 0; i < verts.size(); i++) {
		if (v == verts[i]) {
			return i;
		}
	}
	return -1;
}

void SceneObject::UpdateTransform(float dt)
{
	//r += dt;
	Rotate(0.0f, dt, 0.0f);
	transform = dx::XMMatrixScaling(sx, sy, sz) * dx::XMMatrixRotationX(rx) * dx::XMMatrixRotationY(ry) * dx::XMMatrixRotationZ(rz) * dx::XMMatrixTranslation(x, y, z);
	//transform = dx::XMMatrixScaling(sx, sy, sz) * dx::XMMatrixRotationY(r) * dx::XMMatrixTranslation(x, y, z);
}


