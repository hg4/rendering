#pragma once
#include "Material.h"
class MaterialList :
	public Material
{
public:
	vector<Material*> materialList;
	MaterialList();
	MaterialList(Material * mtr1, Material * mtr2) {
		AddMaterial(mtr1);
		AddMaterial(mtr2);
	}
	~MaterialList();
	void AddMaterial(Material* mtr);
};

