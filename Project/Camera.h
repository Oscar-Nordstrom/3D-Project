#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

class Camera {
public:
	Camera();
	~Camera();
	DirectX::XMMATRIX GetMatrix()const;
	DirectX::XMFLOAT3* GetPos();
	DirectX::XMFLOAT3* GetDir();
	void SetPos(DirectX::XMFLOAT3 pos);
	void SetDir(DirectX::XMFLOAT3 dir);
	void Reset();
private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 direction;
};