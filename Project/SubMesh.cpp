#include "SubMesh.h"


SubMesh::SubMesh(Graphics*& gfx, std::vector<unsigned short>& indices, TextureHandler*& texHandl, std::string name, std::string mtlFile, std::string mtl, int start, int end)
{
	this->gfx = gfx;
	this->texHandl = texHandl;
	this->name = name;
	this->mtlFile = mtlFile;
	this->mtl = mtl;
	this->start = start;
	this->end = end;

	assert(SetUpIndexBuffer(indices) && "Failed to create index buffer.");
	assert(LoadMtl()&&"Failed to load mtl.");
}

SubMesh::~SubMesh()
{
	if (indexBuffer)indexBuffer->Release();
	if (cbuf)cbuf->Release();
	for (int i = 0; i < 3; i++) {
		if (srv[i])srv[i]->Release();
	}
}

void SubMesh::Bind(int flag)
{
	if (flag == NORMAL || flag == CUBE_MAP || flag == CUBE_MAP_TWO) {
		gfx->GetContext()->PSSetShaderResources(0, 3, srv);
		gfx->GetContext()->PSSetConstantBuffers(4, 1, &cbuf);
	}
	gfx->GetContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	gfx->GetContext()->DrawIndexed((UINT)subIndices.size(), 0, 0);
}

bool SubMesh::SetUpIndexBuffer(std::vector<unsigned short>& indices)
{
	int size = (int)indices.size();
	for (int i = 0; i < size; i++) {
		if (i >= start && i <= end) {
			subIndices.push_back(indices[i]);
		}
	}

	D3D11_BUFFER_DESC ibDesc;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;
	ibDesc.ByteWidth = UINT(subIndices.size() * sizeof(unsigned short));
	ibDesc.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = subIndices.data();
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = gfx->GetDevice()->CreateBuffer(&ibDesc, &data, &indexBuffer);

	return !FAILED(hr);
}

bool SubMesh::LoadMtl()
{
	DirectX::XMFLOAT3 neg = { -1.0f, -1.0f, -1.0f };
	float Ns = 0.0f;
	DirectX::XMFLOAT3 kd = { 0.1f, 0.1f, 0.1f };
	DirectX::XMFLOAT3 ks = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 ka = { 0.1f, 0.1f, 0.1f };
	std::string map_Kd = "";
	std::string map_Ks = "";
	std::string map_Ka = "";

	//Read the mtl file
	std::stringstream ss;
	std::ifstream file;
	std::string line;
	std::string prefix = "";
	std::string check;
	bool found = false;
	bool foundMapKd = false;
	bool foundMapKa = false;
	bool foundMapKs = false;
	file.open("../Resources/Mtl/" + mtlFile);
	if (!file.is_open()) {
		std::cerr << "Failed to open mesh file.\n";
		return false;
	}
	while (std::getline(file, line)) {
		ss.clear();
		if (line.size() > 0 && line[line.size() - 1] != ' ') {
			line += " ";
		}
		ss.str(line);
		ss >> prefix;
		if (prefix == "newmtl") {
			ss >> check;
			if (!found) {
				if (check == mtl) {
					found = true;
				}
			}
			else {
				break;
			}

		}
		if (found) {
			if (prefix == "Ns") {
				ss >> Ns;
			}
			else if (prefix == "Kd") {
				ss >> kd.x >> kd.y >> kd.z;
			}
			else if (prefix == "Ks") {
				ss >> ks.x >> ks.y >> ks.z;
			}
			else if (prefix == "Ka") {
				ss >> ka.x >> ka.y >> ka.z;
			}
			else if (prefix == "map_Kd") {
				ss >> map_Kd;
				foundMapKd = true;
			}
			else if (prefix == "map_Ks") {
				ss >> map_Ks;
				foundMapKs = true;
			}
			else if (prefix == "map_Ka") {
				ss >> map_Ka;
				foundMapKa = true;
			}
		}
	}
	Material mat(gfx, texHandl, Ns, kd, ka, ks, map_Kd, map_Ks, map_Ka);
	if (!mat.SetupTexture()) {
		return false;
	}
	if (!mat.CreateSRV(srv)) {
		return false;
	}
	if (!mat.SetUpCbuf(cbuf)) {
		return false;
	}

	gfx->GetContext()->PSSetShaderResources(0, 3, srv);
	gfx->GetContext()->PSSetConstantBuffers(4, 1, &cbuf);

	return true;
}
