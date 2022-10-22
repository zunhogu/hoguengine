#pragma once
#include "ModelComp.h"
#include "TextureClass.h"

class TerrainComp : public ModelComp
{
private:
	string m_heightMapName;
public:
	TerrainComp();
	TerrainComp(const TerrainComp& terrain);
	~TerrainComp();

	void Initialize(TextureClass* m_heightMap, TextureClass* m_colorMap);
	virtual void Shutdown();

	virtual void Render(ModelNode* node);

};

