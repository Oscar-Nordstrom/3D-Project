#include "Material.h"

int roundUpToMul(int numToRound, int multiple)
{
	if (multiple == 0)
		return numToRound;

	int remainder = numToRound % multiple;
	if (remainder == 0)
		return numToRound;

	return numToRound + multiple - remainder;
}

Material::Material(Graphics*& gfx, TextureHandler*& texHandl, float Ns, DirectX::XMFLOAT3 kd, DirectX::XMFLOAT3 ks, DirectX::XMFLOAT3 ka, std::string map_Kd, std::string map_Ks, std::string map_Ka)
	:map_Kd(map_Kd), map_Ks(map_Ks), map_Ka(map_Ka)
{
	theMtlData.Ns = Ns;
	theMtlData.kd = kd;
	theMtlData.ks = ks;
	theMtlData.ka = ka;
	one = -1;
	two = -1;
	three = -1;
	this->gfx = gfx;
	this->texHandl = texHandl;
}

Material::~Material()
{
}

bool Material::SetupTexture()
{
	bool found = false;
	bool found1 = false;
	bool found2 = false;
	//int one, two, three;
	int i = 0;
	int size = texHandl->NumImages();
	for (i = 0; i < size; i++) {
		if (texHandl->GetImage(i).name == map_Kd) {
			found = true;
			one = i;
		}
		else if (texHandl->GetImage(i).name == map_Ks) {
			found1 = true;
			two = i;
		}
		else if (texHandl->GetImage(i).name == map_Ka) {
			found2 = true;
			three = i;
		}
		if (found && found1 && found2) {
			break;
		}
	}
	//Setting deafult texture if it was not found (0 == deafult)
	if (!found) {
		one = 0;
	}
	if (!found1) {
		two = 0;
	}
	if (!found2) {
		three = 0;
	}

	return true;
}

bool Material::CreateSRV(ID3D11ShaderResourceView* srv[])
{
	if (FAILED(gfx->GetDevice()->CreateShaderResourceView(texHandl->GetImage(one).tex, nullptr, &srv[0]))) {
		return false;
	}
	if (FAILED(gfx->GetDevice()->CreateShaderResourceView(texHandl->GetImage(two).tex, nullptr, &srv[1]))) {
		return false;
	}
	if (FAILED(gfx->GetDevice()->CreateShaderResourceView(texHandl->GetImage(three).tex, nullptr, &srv[2]))) {
		return false;
	}
	return true;
}

bool Material::SetUpCbuf(ID3D11Buffer*& cbuf)
{
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = roundUpToMul(sizeof(MtlData), 16);
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &theMtlData;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = gfx->GetDevice()->CreateBuffer(&desc, &data, &cbuf);

	return !FAILED(hr);
}
