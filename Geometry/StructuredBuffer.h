#pragma once

// ������ ���۴� ���� ������ ���ҵ��� ��� ���۷� ������ ���ϸ� �ϳ��� �迭�̴�.
// ������ ���۸� ��� ���̴��� �Է��ڿ����� ���� ���ؼ��� �Է��� ���̴� ���ҽ���, ����� ���� ���� ���ٺ並 ����Ѵ�.
class StructuredBuffer
{
private:
	ID3D11Resource* m_input;
	ID3D11ShaderResourceView* m_srv;

	ID3D11Resource* m_output;
	ID3D11UnorderedAccessView* m_uav;

	ID3D11Resource* m_result;

	void* m_inputData;
	UINT m_inputStride;
	UINT m_inputCount;

	UINT m_outputStride;
	UINT m_outputCount;

public:
	StructuredBuffer(void* inputData, UINT inputStride, UINT inputCount, UINT outputStride = 0, UINT outputCount = 0);
	~StructuredBuffer();

	void Copy(void* data, UINT size);

	ID3D11UnorderedAccessView*& GetUAV() { return m_uav; }
	ID3D11ShaderResourceView*& GetSRV() { return m_srv; }

private:
	void CreateInput();
	void CreateSRV();
	void CreateOutput();
	void CreateUAV();
	void CreateResult();
};