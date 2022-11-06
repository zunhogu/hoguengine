#pragma once

// 구조적 버퍼는 같은 형식의 원소들을 담는 버퍼로 간단히 말하면 하나의 배열이다.
// 구조저 버퍼를 계산 셰이더의 입력자원으로 묶기 위해서는 입력은 셰이더 리소스뷰, 출력은 순서 없는 접근뷰를 사용한다.
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