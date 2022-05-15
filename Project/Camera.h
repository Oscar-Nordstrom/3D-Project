#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

class Camera {
public:
	Camera();
	~Camera();
	DirectX::XMMATRIX GetMatrix();
	DirectX::XMFLOAT3* GetPos();
	DirectX::XMFLOAT3* GetDir();
	DirectX::BoundingFrustum GetFrustum();
	void SetPos(DirectX::XMFLOAT3 pos);
	void SetDir(DirectX::XMFLOAT3 dir);
	void SetUpDir(DirectX::XMFLOAT3 dir);
	void SetProjection(DirectX::XMMATRIX proj);
	void Reset();

	void Rotate(float rot);

	void forward();
	void backward();
	void left();
	void right();
	void up();
	void down();

private:
	void updateFrustum();
	float DegToRad(float deg);
private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 direction;
	DirectX::XMFLOAT3 upDirection;

	DirectX::BoundingFrustum frustum;

public:
	float speed;
};