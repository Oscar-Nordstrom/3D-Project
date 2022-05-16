#pragma once
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "SceneObjectTest.h"
#include "Camera.h"

enum class Node{ROOT, INNER, LEAF};

const int MAXDEPTH = 2;

class QuadTree {
public:
	QuadTree(std::vector<SceneObjectTest*> &objects, int depth, float size, DirectX::XMFLOAT3 pos);
	~QuadTree();
	void InsideNodes(Camera& cam, std::vector<QuadTree*>* list);
	DirectX::BoundingBox GetBox()const;
	void GetPoints(DirectX::XMFLOAT3* points);

private:
	std::vector<SceneObjectTest*> objects;
	DirectX::BoundingBox box;
	QuadTree* nodes[4];
	Node type;
	int depth;
	DirectX::XMFLOAT3 p1, p2;
};