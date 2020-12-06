#include "MaterialList.h"



MaterialList::MaterialList()
{
}


MaterialList::~MaterialList()
{
}

void MaterialList::AddMaterial(Material * mtr)
{
	materialList.push_back(mtr);
	for (int i = 0; i < mtr->textureList.size(); i++) {
		textureList.push_back(mtr->textureList[i]);
	}
}