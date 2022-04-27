#include "Model.h"

Model::Model()
{
	indexBuffer = nullptr;
	inputLayout = nullptr;
	pShader = nullptr;
	hShader = nullptr;
	dShader = nullptr;
	vShader = nullptr;
	cShader = nullptr;
	samState = nullptr;
	shadowSamp = nullptr;
	vertexBuffer = nullptr;
	constantBuffer = nullptr;
	images = nullptr;
	topologyTriList = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	topology = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
}

Model::~Model()
{
	if (vShader)vShader->Release();
	if (hShader)hShader->Release();
	if (dShader)dShader->Release();
	if (pShader)pShader->Release();
	if (cShader)cShader->Release();
	if (inputLayout)inputLayout->Release();
	if (samState)samState->Release();
	if (shadowSamp)shadowSamp->Release();
	if (vertexBuffer)vertexBuffer->Release();
	if (indexBuffer)indexBuffer->Release();
	if (constantBuffer)constantBuffer->Release();

	for (auto o : subs) {
		o->Terminate();
		delete o;
	}

	delete images;
}

bool Model::Load(string obj, string vShaderPath, string hShaderPath, string dShaderPath, string pShaderPath, string cShaderPath, DirectX::XMMATRIX transform, Graphics*& gfx)
{
	assert(LoadShaders(vShaderPath, hShaderPath, dShaderPath, pShaderPath, cShaderPath, gfx), "Failed to load shaders.");
	assert(LoadObj(obj, gfx), "Failed to load OBJ.");
	assert(CreateInputLayout(gfx->GetDevice()), "Failed to create input layout.");
	assert(SetUpSampler(gfx->GetDevice()), "Failed to set up sampler.");
	assert(CreateVertexBuffer(gfx->GetDevice()), "Failed to create vertex buffer.");
	assert(CreateIndexBuffer(gfx->GetDevice()), "Failed to create index buffer.");
	assert(CreateConstantBuffer(*gfx, transform), "Failed to create constant buffer.");


	return true;
}

void Model::Draw(Graphics*& gfx, DirectX::XMMATRIX transform, int flag)
{
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	
	if (flag == NORMAL) {
		gfx->GetContext()->VSSetShader(vShader, nullptr, 0);
		gfx->GetContext()->HSSetShader(hShader, nullptr, 0);
		gfx->GetContext()->DSSetShader(dShader, nullptr, 0);
		gfx->GetContext()->PSSetShader(pShader, nullptr, 0);
		gfx->GetContext()->CSSetShader(cShader, nullptr, 0);

		UpdateCbuf(*gfx, transform);

		gfx->GetContext()->PSSetSamplers(0, 1, &samState);
		gfx->GetContext()->PSSetSamplers(1, 1, &shadowSamp);
		gfx->GetContext()->IASetPrimitiveTopology(topology);
		gfx->GetContext()->DSSetConstantBuffers(0, 1, &constantBuffer);
	}
	else if(flag == SHADOW) {
		gfx->GetContext()->PSSetShader(nullptr, nullptr, 0);
		gfx->GetContext()->HSSetShader(nullptr, nullptr, 0);
		gfx->GetContext()->DSSetShader(nullptr, nullptr, 0);
		gfx->GetContext()->CSSetShader(nullptr, nullptr, 0);

		UpdateCbuf(*gfx, transform);

		gfx->GetContext()->IASetPrimitiveTopology(topologyTriList);
	}
	else if (flag == CUBE_MAP) {
		gfx->GetContext()->VSSetShader(vShader, nullptr, 0);
		gfx->GetContext()->HSSetShader(hShader, nullptr, 0);
		gfx->GetContext()->DSSetShader(dShader, nullptr, 0);
		gfx->GetContext()->PSSetShader(pShader, nullptr, 0);
		//gfx->GetContext()->CSSetShader(nullptr, nullptr, 0);

		UpdateCbuf(*gfx, transform);

		gfx->GetContext()->PSSetSamplers(0, 1, &samState);
		gfx->GetContext()->PSSetSamplers(1, 1, &shadowSamp);
		gfx->GetContext()->IASetPrimitiveTopology(topology);
		gfx->GetContext()->DSSetConstantBuffers(0, 1, &constantBuffer);
	}
	else if (flag == CUBE_MAP_TWO) {
		gfx->GetContext()->VSSetShader(vShader, nullptr, 0);
		gfx->GetContext()->HSSetShader(hShader, nullptr, 0);
		gfx->GetContext()->DSSetShader(dShader, nullptr, 0);
		//gfx->GetContext()->PSSetShader(pShader, nullptr, 0);
		//gfx->GetContext()->CSSetShader(cShader, nullptr, 0);

		UpdateCbuf(*gfx, transform);

		gfx->GetContext()->PSSetSamplers(0, 1, &samState);
		gfx->GetContext()->PSSetSamplers(1, 1, &shadowSamp);
		gfx->GetContext()->IASetPrimitiveTopology(topology);
		gfx->GetContext()->DSSetConstantBuffers(0, 1, &constantBuffer);
	}

	gfx->GetContext()->IASetInputLayout(inputLayout);
	gfx->GetContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	gfx->GetContext()->VSSetConstantBuffers(0, 1, &constantBuffer);



	if (subs.size() > 0) {
		for (auto& o : subs) {
			o->Bind(gfx->GetContext(), flag);
		}
	}
	else {

		gfx->GetContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

		gfx->GetContext()->DrawIndexed((UINT)indices.size(), 0, 0);
	}
}

bool Model::LoadShaders(string vShaderPath, string hShaderPath, string dShaderPath, string pShaderPath, string cShaderPath, Graphics*& gfx)
{

	if (!ReadShader(gfx, vShaderPath, VERTEX_SHADER, vShader, hShader, dShader, pShader, cShader) && vShaderPath != NO_SHADER) {
		return false;
	}
	if (!ReadShader(gfx, hShaderPath, HULL_SHADER, vShader, hShader, dShader, pShader, cShader) && hShaderPath != NO_SHADER) {
		return false;
	}
	if (!ReadShader(gfx, dShaderPath, DOMAIN_SHADER, vShader, hShader, dShader, pShader, cShader) && dShaderPath != NO_SHADER) {
		return false;
	}
	if (!ReadShader(gfx, pShaderPath, PIXEL_SHADER, vShader, hShader, dShader, pShader, cShader) && pShaderPath != NO_SHADER) {
		return false;
	}
	if (!ReadShader(gfx, cShaderPath, COMPUTE_SHADER, vShader, hShader, dShader, pShader, cShader) && cShaderPath != NO_SHADER) {
		return false;
	}

	return true;
}

bool Model::LoadObj(string obj, Graphics*& gfx)
{
	bool foundMtllib = false;

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

	map<string, int> verts_map;

	ifstream file(obj);
	if (!file.is_open()) {
		return false;
	}
	while (getline(file, line)) {

		ss.clear();
		if (line.size() > 0 && line[line.size() - 1] != ' ') {
			line += " ";
		}
		ss.str(line);

		ss >> prefix;

		if (!foundMtllib && prefix == "mtllib") {
			
			ss >> mtlFile;
			images = new MtlImages(mtlFile, gfx->GetDevice());
			foundMtllib = true;
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
			if (useSubs) {
				submeshEnd = (int)indices.size() - 1;
				SubMesh* subM = new SubMesh(gfx->GetDevice(), gfx->GetContext(), indices, images, subName, mtlFile, mtl, submeshStart, submeshEnd);
				subs.push_back(subM);
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

					//Works for smaller files, but for larger like sponza something goes wrong
					/*auto found_it = verts_map.find(vertTemp.make_this_string());
					if (verts_map.end() == found_it) {
						verts.push_back(vertTemp);
						int indi = (int)verts.size() - 1;
						verts_map.insert(std::make_pair(vertTemp.make_this_string(), indi));
						indices.push_back(indi);
					}
					else {
						indices.push_back(found_it->second);
					}*/
					verts.push_back(vertTemp);
					int indi = (int)verts.size() - 1;
					indices.push_back(indi);

					if (submesh) {
						submesh = false;
						submeshStart = (int)indices.size() - 1;
						useSubs = true;
					}
				}
			}
		}

	}
	if (useSubs) {
		useSubs = false;
		submeshEnd = (int)indices.size() - 1;
		SubMesh* subM = new SubMesh(gfx->GetDevice(), gfx->GetContext(), indices, images, subName, mtlFile, mtl, submeshStart, submeshEnd);
		subs.push_back(subM);
	}

	/*For debugging
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
	if (FAILED(hr)) {
		return false;
	}

	sampler.Filter = D3D11_FILTER_MINIMUM_ANISOTROPIC;

	hr = device->CreateSamplerState(&sampler, &shadowSamp);

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

bool Model::ReadShader(Graphics*& gfx, string path, int flag, ID3D11VertexShader*& v, ID3D11HullShader*& h, ID3D11DomainShader*& d, ID3D11PixelShader*& p, ID3D11ComputeShader*& c)
{

	std::string shaderData;
	std::ifstream reader;
	reader.open(path, std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open PS file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);//Go to the end of the file
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));//Reserve space based on the size of the file
	reader.seekg(0, std::ios::beg);//Go to the start of the file

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());//Assign the file to the shader data

	switch (flag)
	{
	case VERTEX_SHADER:
		if (FAILED(gfx->GetDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &v)))
		{
			std::cerr << "Failed to create pixel shader!" << std::endl;
			return false;
		}
		vShaderByteCode = shaderData;
		break;
	case HULL_SHADER:
		if (FAILED(gfx->GetDevice()->CreateHullShader(shaderData.c_str(), shaderData.length(), nullptr, &h)))
		{
			std::cerr << "Failed to create pixel shader!" << std::endl;
			return false;
		}
		break;
	case DOMAIN_SHADER:
		if (FAILED(gfx->GetDevice()->CreateDomainShader(shaderData.c_str(), shaderData.length(), nullptr, &d)))
		{
			std::cerr << "Failed to create pixel shader!" << std::endl;
			return false;
		}
		break;
	case PIXEL_SHADER:
		if (FAILED(gfx->GetDevice()->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &p)))
		{
			std::cerr << "Failed to create pixel shader!" << std::endl;
			return false;
		}
		break;
	case COMPUTE_SHADER:
		if (FAILED(gfx->GetDevice()->CreateComputeShader(shaderData.c_str(), shaderData.length(), nullptr, &c)))
		{
			std::cerr << "Failed to create pixel shader!" << std::endl;
			return false;
		}
		break;
	}
	//Create the pixel shader and store it in pShader
	

	shaderData.clear();//Clear the string with data
	reader.close();//Close the file

	return true;
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
	if (FAILED(hr)) {
		assert(false, "Failed to update constant buffer.");
	}

	return true;
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
