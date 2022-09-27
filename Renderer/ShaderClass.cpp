#include "pch.h"
#include "ShaderClass.h"

ShaderClass::ShaderClass() {
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_sampleState = 0;
}


ShaderClass::ShaderClass(const ShaderClass& other) {}


ShaderClass::~ShaderClass() {
	if (m_matrixBuffer) {
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}
	// 레이아웃을 해제한다.
	if (m_layout) {
		m_layout->Release();
		m_layout = 0;
	}

	// 픽셀 셰이더를 해제한다.
	if (m_pixelShader) {
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// 정점 셰이더를 해제한다.
	if (m_vertexShader) {
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
	// 텍스처 샘플러를 해제한다.
	if (m_sampleState) {
		m_sampleState->Release();
		m_sampleState = 0;
	}
}

//OutputShaderErrorMessage 함수에서는 정점 / 픽셀 셰이더의 컴파일 도중 오류로 인해 만들어진 에러 메세지를 출력하는 일을 한다.
void ShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// 에러 메세지를 담고 있는 문자열 버퍼의 포인터를 가져온다.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// 메세지의 길이를 가져온다.
	bufferSize = errorMessage->GetBufferSize();

	// 파일을 열고 안에 메세지를 기록한다.
	fout.open("shader-error.txt");

	// 에러 메세지를 쓴다.
	for (i = 0; i < bufferSize; i++) {
		fout << compileErrors[i];
	}

	// 파일을 닫는다.
	fout.close();

	// 에러 메세지를 반환한다.
	errorMessage->Release();
	errorMessage = 0;

	// 컴파일 에러가 있음을 팝업 메세지로 알려준다.
	MessageBox(hwnd, "Error compiling shader.  Check shader-error.txt for message.", nullptr, MB_OK);

	return;
}