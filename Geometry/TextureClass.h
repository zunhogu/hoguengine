#pragma once
#include "ResourceClass.h"
#include "TextureBuffer.h"

class TextureClass : public ResourceClass
{
protected:
    ID3D11Device* m_device;

    ID3D11Texture2D* m_texture;
    ID3D11ShaderResourceView* m_shaderResourceView;

    TextureBuffer* m_textureBuffer;

    wstring m_filePath;
    wstring m_fileExtension;
public:
    TextureClass();
    virtual ~TextureClass();

public:
    virtual void Initialize(const wstring& _strFilePath);
    void Shutdown();

    DirectX::ScratchImage LoadTexture(const wstring& _strFilePath);
    bool SaveTexture();

    ID3D11Texture2D* GetTexture() { return m_texture; }
    ID3D11ShaderResourceView*& GetShaderResourceView() { return m_shaderResourceView; }

    TextureBuffer* GetTextureBuffer() { return m_textureBuffer; }
    void Copy(ID3D11Resource* resource);

    friend class ResMgrClass;
};

