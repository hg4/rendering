#pragma once
#include "Material.h"
#include <memory>
class MaterialList :
	public Material
{
public:
	vector<shared_ptr<Material>> materialList;
	MaterialList();
	MaterialList(shared_ptr<Material> mtr1, shared_ptr<Material> mtr2) {
		AddMaterial(mtr1);
		AddMaterial(mtr2);
	}
	~MaterialList();
	void AddMaterial(shared_ptr<Material> mtr);
};

