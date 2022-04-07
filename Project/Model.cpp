#include "Model.h"

Model::Model()
{
	indexBuffer = nullptr;
	inputLayout = nullptr;
	pShader = nullptr;
	vShader = nullptr;
	cShader = nullptr;
	samState = nullptr;
	vertexBuffer = nullptr;
	constantBuffer = nullptr;
	images = nullptr;
	topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

Model::~Model()
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

	delete images;
}

bool Model::Load(string obj, string vShader, string pShader, string cShader, DirectX::XMMATRIX transform, Graphics*& gfx)
{
	
	if (!LoadShaders(vShader, pShader, cShader, gfx)) {
		return false;
	}
	if (!LoadObj(obj, gfx)) {
		return false;
	}
	if (!CreateInputLayout(gfx->GetDevice())) {
		std::cerr << "Failed to create input layout.\n";
		return false;
	}
	if (!SetUpSampler(gfx->GetDevice())) {
		std::cerr << "Failed to create sampler.\n";
		return false;
	}
	if (!CreateVertexBuffer(gfx->GetDevice())) {
		std::cerr << "Failed to create vertex buffer.\n";
		return false;
	}
	if (!CreateIndexBuffer(gfx->GetDevice())) {
		std::cerr << "Failed to create index buffer.\n";
		return false;
	}
	if (!CreateConstantBuffer(*gfx, transform)) {
		std::cerr << "Failed to create constant buffer.\n";
		return false;
	}

	return true;
}


void Model::Draw(Graphics*& gfx, DirectX::XMMATRIX transform, bool withShaders)
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
		UpdateCbuf(*gfx, transform);
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

bool Model::LoadShaders(string vShaderPath, string pShaderPath, string cShaderPath, Graphics*& gfx)
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
	if (FAILED(gfx->GetDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &vShader)))
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
	if (FAILED(gfx->GetDevice()->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pShader)))
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
	if (FAILED(gfx->GetDevice()->CreateComputeShader(shaderData.c_str(), shaderData.length(), nullptr, &cShader)))
	{
		std::cerr << "Failed to create pixel shader!" << std::endl;
		return false;
	}

	return true;
}

bool Model::LoadObj(string obj, Graphics*& gfx)
{

	bool submesh = false;
	bool useSubs = false;
	int submeshStart = 0;
	int submeshEnd = 0;
	string mtl = "";
	string subName;
	string mtlFile;

	string prefix;
	string line;
	stringstream ss;

	DirectX::XMFLOAT3 tempV, tempVn;
	DirectX::XMFLOAT2 tempVt;
	int tempI = 0;
	int step = 0;

	//map<string, SimpleVertex> verts_map;
	map<string, int> verts_map;

	ifstream file(obj);
	if (!file.is_open()) {
		return false;
	}
	while (getline(file, line)) {

		ss.clear();
		if (line[line.size() - 1] != ' ') {
			line += " ";
		}
		ss.str(line);

		ss >> prefix;

		if (prefix == "mtllib") {
			
			ss >> mtlFile;
			images = new MtlImages(mtlFile);
		}
		else if (prefix == "usemtl") {
			ss >> mtl;
		}
		else if (prefix == "v") {
			ss >> tempV.x >> tempV.y >> tempV.z;
			v.push_back(tempV);
		}
		else if (prefix == "vn") {
			ss >> tempVn.x >> tempVn.y >> tempVn.z;
			vn.push_back(tempVn);
		}
		else if (prefix == "vt") {
			ss >> tempVt.x >> tempVt.y;
			vt.push_back(tempVt);
		}
		else if (prefix == "g") {
			if (subs.size() > 0 || useSubs) {
				submeshEnd = indices.size() - 1;
				SubMesh* subM = new SubMesh(gfx->GetDevice(), gfx->GetContext(), indices, images, subName, mtlFile, mtl, submeshStart, submeshEnd);
				subs.push_back(*subM);
			}
			submesh = true;
			ss >> subName;
		}
		else if (prefix == "f") {

			if (step++ > 4) {
				step = step;
			}
			int count = 0;
			int i = 0;
			while (ss >> tempI) {
				i = tempI - 1;
				switch (count)
				{
				case 0:
					tempV = v[i];
					break;
				case 1:
					tempVt = vt[i];
					break;
				case 2:
					tempVn = vn[i];
					break;
				}
				if (ss.peek() == '/' || ss.peek() == ' ') {
					count++;
					ss.ignore(1, '/');
				}
				if (count > 2) {
					count = 0;
					SimpleVertex vertTemp(tempV, tempVn, tempVt);

					auto found_it = verts_map.find(vertTemp.make_this_string());
					if (verts_map.end() == found_it) {
						verts.push_back(vertTemp);
						int indi = (int)verts.size() - 1;
						verts_map.insert(std::make_pair(vertTemp.make_this_string(), indi));
						indices.push_back(indi);
					}
					else {
						indices.push_back(found_it->second);
					}
					if (submesh) {
						submesh = false;
						submeshStart = indices.size() - 1;
						useSubs = true;
					}
				}
			}
		}

	}
	if (subs.size() > 0) {
		submeshEnd = indices.size() - 1;
		SubMesh* subM = new SubMesh(gfx->GetDevice(), gfx->GetContext(), indices, images, subName, mtlFile, mtl, submeshStart, submeshEnd);
		subs.push_back(*subM);
	}

	//For debugging
	//int jfjd = 0;
	//subs;
	//images;
	/*verts;
	indices;
	if (FindVert() != -1) {
		assert(false && "THIS WAS SAD");
	}*/
	return true;
}


bool Model::CreateInputLayout(ID3D11Device*& device)
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

bool Model::SetUpSampler(ID3D11Device*& device)
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

bool Model::CreateVertexBuffer(ID3D11Device*& device)
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

bool Model::CreateIndexBuffer(ID3D11Device*& device)
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

bool Model::CreateConstantBuffer(Graphics& gfx, DirectX::XMMATRIX transform)
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

bool Model::UpdateCbuf(Graphics& gfx, DirectX::XMMATRIX transform)
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

/*int Model::FindVert()
{
	for (int i = 0; i < verts.size()-1; i++) {
		for (int j = i+1; j < verts.size(); j++) {
			if (verts[i] == verts[j]) {
				return i;
			}
		}
	}

	return -1;
}*/
