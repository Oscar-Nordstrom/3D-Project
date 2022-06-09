#include "FileLoader.h"

FileLoader::FileLoader()
{
	this->texHandl = nullptr;
}

FileLoader::~FileLoader()
{
	for (auto p : subs) {
		//if (p != nullptr)delete p;
	}

}

bool FileLoader::LoadObj(std::string filePath, Graphics* gfx)
{
	bool foundMtllib = false;

	bool submesh = false;
	bool useSubs = false;
	int submeshStart = 0;
	int submeshEnd = 0;
	std::string mtl = "";
	std::string subName;
	std::string mtlFile;

	std::string prefix;
	std::string line;
	std::stringstream ss;

	std::vector<DirectX::XMFLOAT3> v;
	std::vector<DirectX::XMFLOAT3> vn;
	std::vector<DirectX::XMFLOAT2> vt;

	DirectX::XMFLOAT3 tempV, tempVn;
	DirectX::XMFLOAT2 tempVt;
	int tempI = 0;
	int step = 0;

	std::map<std::string, int> verts_map;

	std::ifstream file(filePath);
	if (!file.is_open()) {
		return false;
	}
	while (getline(file, line)) {

		ss.clear();
		if (line.size() > 0 && line[line.size() - 1] != ' ') {
			line += " ";
		}
		ss.str(line);

		ss >> prefix;

		if (!foundMtllib && prefix == "mtllib") {

			ss >> mtlFile;
			//images = new MtlImages(mtlFile, gfx->GetDevice(), texHandl);
			LoadImages(mtlFile, gfx);
			foundMtllib = true;
		}
		else if (prefix == "usemtl") {
			ss >> mtl;
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
			if (useSubs) {
				submeshEnd = (int)indices.size() - 1;
				SubMesh* subM = new SubMesh(gfx, indices, texHandl, subName, mtlFile, mtl, submeshStart, submeshEnd);
				subs.push_back(subM);
			}
			submesh = true;
			ss >> subName;
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

					//Checking for duplicates
					auto found_it = verts_map.find(vertTemp.make_this_string());
					if (verts_map.end() == found_it) {
						verts.push_back(vertTemp);
						int indi = (int)verts.size() - 1;
						verts_map.insert(std::make_pair(vertTemp.make_this_string(), indi));
						indices.push_back(indi);
					}
					else {
						indices.push_back(found_it->second);
					}
					//Not checking for duplicates
					//verts.push_back(vertTemp);
					//int indi = (int)verts.size() - 1;
					//indices.push_back(indi);

					if (submesh) {
						submesh = false;
						submeshStart = (int)indices.size() - 1;
						useSubs = true;
					}
				}
			}
		}

	}
	if (useSubs) {
		useSubs = false;
		submeshEnd = (int)indices.size() - 1;
		SubMesh* subM = new SubMesh(gfx, indices, texHandl, subName, mtlFile, mtl, submeshStart, submeshEnd);
		subs.push_back(subM);
	}

	/*For debugging
	if (FindVert() != -1) {
		assert(false && "THIS WAS SAD");
	}*/
	return true;
}

void FileLoader::SetTexHandl(TextureHandler*& texHandl)
{
	this->texHandl = texHandl;
}

std::vector<SimpleVertex> FileLoader::GetVerts()
{
	return verts;
}

std::vector<unsigned short> FileLoader::GetIndices()
{
	return indices;
}

std::vector<SubMesh*> FileLoader::GetSubs()
{
	return subs;
}

bool FileLoader::AddImage(std::string what, Graphics* gfx)
{
	ImageData data;
	if (!texHandl->TextureExists(what)) {
		if (!texHandl->AddTexture(what, gfx->GetDevice())) {
			return false;
		}
		data = texHandl->GetImage(what);
	}
	else {
		data = texHandl->GetImage(what);
	}

	textures.push_back(data.tex);

	return true;
}

bool FileLoader::LoadImages(std::string filePath, Graphics* gfx)
{
	std::stringstream ss;
	std::ifstream file;
	std::string line;
	std::string prefix = "";
	std::string temp;


	if (AddImage("Default.png", gfx)) {
		names.push_back("Default");
	};

	file.open("../Resources/Mtl/" + filePath);
	if (!file.is_open()) {
		std::cerr << "Failed to open mesh file.\n";
		return false;
	}
	while (std::getline(file, line)) {
		ss.clear();
		if (line.size() > 0 && line[line.size() - 1] != ' ') {
			line += " ";
		}
		ss.str(line);
		ss >> prefix;
		if (prefix == "map_Kd") {
			ss >> temp;
			names.push_back(temp);
			AddImage(temp, gfx);
		}
		else if (prefix == "map_Ks") {
			ss >> temp;
			names.push_back(temp);
			AddImage(temp, gfx);
		}
		else if (prefix == "map_Ka") {
			ss >> temp;
			names.push_back(temp);
			AddImage(temp, gfx);
		}
	}
	return true;
}

