#pragma once
#include "ResourceClass.h"


class TextureClass :
    public ResourceClass
{
private:
    ID3D11ShaderResourceView* m_Texture;
public:
    TextureClass();
    ~TextureClass();

public:
    void Initialize(ID3D11Device* device, const wstring& _strFilePath);
    void Shutdown();

    DirectX::ScratchImage LoadTexture(const wstring& _strFilePath);

    ID3D11ShaderResourceView* GetTexture() { return m_Texture; }

    friend class ResMgrClass;
};

