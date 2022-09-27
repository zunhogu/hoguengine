#pragma once
#include "ResourceItem.h"

class MYDLL_DECLSPEC SoundItem : public ResourceItem
{
private:
public:
	SoundItem();
	SoundItem(char* id, char* path);
	~SoundItem();
};

