#pragma once
#include "ResourceItem.h"

class MYDLL_DECLSPEC ModelItem : public ResourceItem
{
private:
public:
	ModelItem();
	ModelItem(char* id, char* path);
	~ModelItem();
};

