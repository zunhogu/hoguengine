#pragma once
#include "ResourceClass.h"


class TextureClass : public ResourceClass
{
protected:
    ID3D11Device* m_device;
    ID3D11ShaderResourceView* m_resourceView;

public:
    TextureClass();
    virtual ~TextureClass();

public:
    virtual void Initialize(const wstring& _strFilePath);
    void Shutdown();

    DirectX::ScratchImage LoadTexture(const wstring& _strFilePath);

    ID3D11ShaderResourceView* GetTexture() { return m_resourceView; }

    friend class ResMgrClass;
};

