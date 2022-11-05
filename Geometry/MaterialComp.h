#pragma once
#include "ModelComp.h"
#include "Material.h"
#include "RenderTextureClass.h"
#include "BitMapClass.h"
class MaterialComp : public ModelComp
{
private:
	wstring m_materialName;

	Material* m_material;

	BitMapClass* m_bitmap;
	RenderTextureClass* m_renderTexture;

public:
	MaterialComp();
	MaterialComp(const MaterialComp& material);
	~MaterialComp();

	bool Initialize();
	wstring GetMaterialName() { return m_materialName; }
	void SetMateriaName(wstring name) { m_materialName = name; }

	Material* GetMaterial() { return m_material; }
	void SetMaterial(Material* material) { m_material = material; }

	virtual void Render(ModelNode* node);
	bool RenderMaterial(ID3D11DeviceContext* deviceContext, XMMATRIX baseViewMatrix);

	ID3D11ShaderResourceView* GetShaderResourceView();
};

