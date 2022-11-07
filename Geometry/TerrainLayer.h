#pragma once

#include "TextureClass.h"
#include "RenderTextureClass.h"
#include "BitMapClass.h"
#include "MaterialComp.h"
#include "ModelComp.h"

class TerrainLayer : ModelComp
{
private:
	wstring m_maskID;
	MaterialComp* m_material1;
	MaterialComp* m_material2;
	MaterialComp* m_material3;

	BitMapClass* m_bitmap;
	RenderTextureClass* m_alphaMapBoard;
	TextureBuffer* m_textureBuffer;

public:
	TerrainLayer();
	~TerrainLayer();
	TerrainLayer(const TerrainLayer&);

	void Shutdown();

	wstring GetMaskID() { return m_maskID; }
	void SetMaskID(wstring id) { m_maskID = id; }
	MaterialComp* GetMaterialComp1() { return m_material1; }
	void SetMaterialComp1(MaterialComp* material) { m_material1 = material; }
	MaterialComp* GetMaterialComp2() { return m_material2; }
	void SetMaterialComp2(MaterialComp* material) { m_material2 = material; }
	MaterialComp* GetMaterialComp3() { return m_material3; }
	void SetMaterialComp3(MaterialComp* material) { m_material3 = material; }

	TextureBuffer* GetTexturebuffer() { return m_textureBuffer; }
	bool SetTextureBuffer(TextureClass* texture);

	BitMapClass* GetBitmap() { return m_bitmap; }
	RenderTextureClass* GetAlphaMapBoard() { return m_alphaMapBoard; }
};

