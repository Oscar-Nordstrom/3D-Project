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
	p1 = DirectX::XMFLOAT3(pos.x - (size / 2), pos.y - size, pos.z - (size / 2));
	p2 = DirectX::XMFLOAT3(pos.x + (size / 2), pos.y + size, pos.z + (size / 2));
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&p1);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&p2);
	DirectX::BoundingBox::CreateFromPoints(this->box, v1, v2);

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
				newPos = DirectX::XMFLOAT3(pos.x - (newSize / 2), pos.y, pos.z - (newSize / 2));
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

void QuadTree::InsideNodes(Camera& cam, std::vector<QuadTree*>* list)
{
	if (cam.Intersect(this->box)) {
		if (this->type != Node::LEAF) {
			for (int i = 0; i < 4; i++) {
				nodes[i]->InsideNodes(cam, list);
			}
		}
		else {
			list->push_back(this);
		}
		//list->push_back(this);
	}
}

DirectX::BoundingBox QuadTree::GetBox() const
{
	return this->box;
}

void QuadTree::GetPoints(DirectX::XMFLOAT3* points)
{
	points[0] = p1;
	points[1] = p2;
}

std::vector<SceneObjectTest*> QuadTree::GetObjects()
{
	return objects;
}
