#pragma once
#include <DirectXMath.h>

class Camera {

public:
	Camera();
	~Camera();

	const DirectX::XMMATRIX& GetMatrix() const;

	const DirectX::XMVECTOR& GetPositionVector() ;
	const DirectX::XMFLOAT3& GetPositionFloat3() ;
	const DirectX::XMVECTOR& GetRotationVector() ;
	const DirectX::XMFLOAT3& GetRotationFloat3() ;

	void SetPosition( DirectX::XMVECTOR& pos);
	void SetPosition( DirectX::XMFLOAT3 pos);
	void Move( DirectX::XMVECTOR& pos);
	void Move( DirectX::XMFLOAT3 pos);
	void SetRotation( DirectX::XMVECTOR& rot);
	void SetRotation( DirectX::XMFLOAT3 rot);
	void Rotate( DirectX::XMVECTOR& rot);
	void Rotate( DirectX::XMFLOAT3 rot);
	void SetLookAtPos(DirectX::XMFLOAT3 lookAtPos);
	 DirectX::XMVECTOR& GetForwardVector();
	 DirectX::XMVECTOR& GetRightVector();
	 DirectX::XMVECTOR& GetBackwardVector();
	 DirectX::XMVECTOR& GetLeftVector();

private:
	void UpdateViewMatrix();

	DirectX::XMVECTOR positionVector;
	DirectX::XMVECTOR rotationVector;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMMATRIX viewMatrix;

	 DirectX::XMVECTOR DEFAULT_FORWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	 DirectX::XMVECTOR DEFAULT_UP_VECTOR = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	 DirectX::XMVECTOR DEFAULT_BACKWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	 DirectX::XMVECTOR DEFAULT_LEFT_VECTOR = DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	 DirectX::XMVECTOR DEFAULT_RIGHT_VECTOR = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	DirectX::XMVECTOR forward;
	DirectX::XMVECTOR left;
	DirectX::XMVECTOR right;
	DirectX::XMVECTOR backward;
};



//Old
/*#include <d3d11.h>
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

	void Forward();
	void Backward();
	void Left();
	void Right();
	void Up();
	void Down();


private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 direction;

	float speed;
};*/