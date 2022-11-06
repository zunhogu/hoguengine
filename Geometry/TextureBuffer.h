#pragma once

class TextureClass;

class TextureBuffer
{
private:
	ID3D11Texture2D* m_output;
	ID3D11ShaderResourceView* m_output_srv;
	ID3D11UnorderedAccessView* m_output_uav;
public:
	TextureBuffer();
	TextureBuffer(TextureClass* texture);
	~TextureBuffer();

	ID3D11Texture2D* GetOutput() { return m_output; }
	ID3D11ShaderResourceView*& GetSRV() { return m_output_srv; }
	ID3D11UnorderedAccessView*& GetUAV() { return m_output_uav; }
};

