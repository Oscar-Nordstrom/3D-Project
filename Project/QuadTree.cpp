#include "QuadTree.h"

QuadTree::QuadTree(std::vector<SceneObjectTest*>& objects, int depth, float size, DirectX::XMFLOAT3 pos)
	:depth(depth)
{
	for (int i = 0; i < 4; i++) {
		nodes[i] = nullptr;
	}
	//Check node type
	switch (this->depth)
	{
	case 0://Root node
		this->type = Node::ROOT;
		break;
	case MAXDEPTH://Leaf node
		this->type = Node::LEAF;
		break;
	default://Inner node
		this->type = Node::INNER;
		break;
	}

	//Set bounding box position and size
	DirectX::XMFLOAT3 f1 = DirectX::XMFLOAT3(pos.x - (size / 2), pos.y - size, pos.z - (size / 2));
	DirectX::XMFLOAT3 f2 = DirectX::XMFLOAT3(pos.x + (size / 2), pos.y + size, pos.z + (size / 2));
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&f1);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&f2);
	this->box.CreateFromPoints(this->box, v1, v2);

	if (this->type == Node::LEAF) {
		//Add game objects that intercet this node
		for (auto p : objects) {
			if (this->box.Intersects(p->GetBoundingSphere())) {
				this->objects.push_back(p);
			}
		}
	}
	else {
		//Create a new QuadTree with added depth and smaller size
		DirectX::XMFLOAT3 newPos;
		float newSize = size / 2;
		for (int i = 0; i < 4; i++) {
			switch (i)
			{
			case 0://Top left
				newPos = DirectX::XMFLOAT3(pos.x - (newSize / 2), pos.y, pos.z + (newSize / 2));
				break;
			case 1://Top right
				newPos = DirectX::XMFLOAT3(pos.x + (newSize / 2), pos.y, pos.z + (newSize / 2));
				break;
			case 2://Bottom left
				newPos = DirectX::XMFLOAT3(pos.x - (newSize / 2), pos.y, pos.z + (newSize / 2));
				break;
			case 3://Bottom right
				newPos = DirectX::XMFLOAT3(pos.x + (newSize / 2), pos.y, pos.z - (newSize / 2));
				break;
			}
			nodes[i] = new QuadTree(objects, depth + 1, newSize, newPos);
		}
	}

}

QuadTree::~QuadTree()
{
	for (int i = 0; i < 4; i++) {
		if (nodes[i] != nullptr) {
			delete nodes[i];
		}
	}
}
