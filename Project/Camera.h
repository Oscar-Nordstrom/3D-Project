#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

class Camera {
public:
	Camera();
	~Camera();
	DirectX::XMMATRIX GetMatrix();
	DirectX::XMFLOAT3* GetPos();
	DirectX::XMFLOAT3* GetDir();
	void SetPos(DirectX::XMFLOAT3 pos);
	void SetDir(DirectX::XMFLOAT3 dir);
	void Reset();

	void Move(DirectX::XMFLOAT3 vec);


private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 direction;
};