#pragma once

#include "Structures.h"
#include "Graphics.h"
#include "TextureHandler.h"

class Material {
public:
	Material(Graphics*& gfx, TextureHandler*& texHandl, float Ns, DirectX::XMFLOAT3 kd, DirectX::XMFLOAT3 ks, DirectX::XMFLOAT3 ka, std::string map_Kd, std::string map_Ks, std::string map_Ka);
	~Material();
	bool SetupTexture();
	bool CreateSRV(ID3D11ShaderResourceView* srv[]);
	bool SetUpCbuf(ID3D11Buffer*& cbuf);
private:
	Graphics* gfx;
	TextureHandler* texHandl;
	MtlData theMtlData;
	std::string map_Kd, map_Ks, map_Ka;
	int one, two, three;
};