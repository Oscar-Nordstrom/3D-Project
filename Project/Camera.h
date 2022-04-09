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

	void Move(DirectX::XMVECTOR vec);

	DirectX::XMVECTOR GetLeftVector();
	DirectX::XMVECTOR GetRightVector();
	DirectX::XMVECTOR GetForwardVector();
	DirectX::XMVECTOR GetBackwardVector();

private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 direction;

	DirectX::XMVECTOR left;
	DirectX::XMVECTOR right;
	DirectX::XMVECTOR forward;
	DirectX::XMVECTOR backward;

	const DirectX::XMVECTOR DefaultLeft = DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR DefaultRight = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR DefaultForward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const DirectX::XMVECTOR DefaultBackward = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const DirectX::XMVECTOR DefaultUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR DefaultDown = DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
};