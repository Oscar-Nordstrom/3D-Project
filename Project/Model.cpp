#include "Model.h"

Model::Model()
{
}

Model::~Model()
{
	if (vShader)vShader->Release();
	if (pShader)pShader->Release();
	if (cShader)cShader->Release();
	if (inputLayout)inputLayout->Release();
	if (samState)samState->Release();
	if (vertexBuffer)vertexBuffer->Release();
	if (indexBuffer)indexBuffer->Release();
	if (constantBuffer)constantBuffer->Release();
}

bool Model::Load(string obj, string vShader, string pShader, string cShader, Graphics*& gfx)
{
	if (!LoadShaders(vShader, pShader, cShader)) {
		return false;
	}
	if (!LoadObj(obj)) {
		return false;
	}

	return true;
}

bool Model::LoadShaders(string vShader, string pShader, string cShader)
{
	return true;
}

bool Model::LoadObj(string obj)
{
	bool subMesh = false;
	string prefix;
	string line;
	stringstream ss;

	DirectX::XMFLOAT3 tempV, tempVn;
	DirectX::XMFLOAT2 tempVt;
	int tempI = 0;
	int step = 0;
	ifstream file(obj);
	if (!file.is_open()) {
		return false;
	}
	while (getline(file, line)) {
		
		ss.clear();
		if (line[line.size() - 1] != ' ') {
			line += " ";
		}
		ss.str(line);

		ss >> prefix;

		if (prefix == "mtllib") {
			//Read mtl file and load data (Should be first thing in the file)
		}
		else if (prefix == "v") {
			ss >> tempV.x >> tempV.y >> tempV.z;
			v.push_back(tempV);
		}
		else if (prefix == "vn") {
			ss >> tempVn.x >> tempVn.y >> tempVn.z;
			vn.push_back(tempVn);
		}
		else if (prefix == "vt") {
			ss >> tempVt.x >> tempVt.y;
			vt.push_back(tempVt);
		}
		else if (prefix == "g") {

		}
		else if (prefix == "f") {

			if (step++ > 4) {
				step = step;
			}
			int count = 0;
			int i = 0;
			while (ss >> tempI) {
				i = tempI - 1;
				switch (count)
				{
				case 0:
					tempV = v[i];
					break;
				case 1:
					tempVt = vt[i];
					break;
				case 2:
					tempVn = vn[i];
					break;
				}
				if (ss.peek() == '/' || ss.peek() == ' ') {
					count++;
					ss.ignore(1, '/');
				}
				if (count > 2) {
					count = 0;
					SimpleVertex vertTemp(tempV, tempVn, tempVt);
					//int index = FindVert(vertTemp);
					//if (index == -1) {
						verts.push_back(vertTemp);
						int indi = (int)verts.size() - 1;
						indices.push_back(indi);
					//}
					//else {
					//	indices.push_back(index);
					//}
				}
			}
		}

	}
	return true;
}

int Model::FindVert(SimpleVertex vert)
{
	for (int i = 0; i < verts.size(); i++) {
		if (verts[i] == vert) {
			return i;
		}
	}
	return -1;
}
