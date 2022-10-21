#pragma once
#include "ModelComp.h"

class TerrainComp : public ModelComp
{
private:
public:
	TerrainComp();
	TerrainComp(const TerrainComp& terrain);
	~TerrainComp();

	virtual void Shutdown();

	virtual void Render(ModelNode* node);

};

