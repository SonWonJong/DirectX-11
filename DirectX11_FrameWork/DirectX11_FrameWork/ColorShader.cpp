#include "stdafx.h"
#include "ColorShader.h"


ColorShader::ColorShader()
{
}


ColorShader::~ColorShader()
{
}

bool ColorShader::Initialize(ID3D11Device* InDevice, HWND InHwnd)
{
	// 정점 및 픽셀 쉐이더를 초기화.
	return InitializeShader(InDevice, InHwnd, L"../DirectX11_FrameWork/color.vs", L"../DirectX11_FrameWork/color.ps");
}

bool ColorShader::InitializeShader(ID3D11Device* InDevice, HWND InHwnd, const WCHAR* InVertexFilename, const WCHAR* InPixelFilename)
{
	ID3D10Blob* ErrorMessage = nullptr;

	// 버텍스 쉐이더 코드를 컴파일
	ID3D10Blob* VertexShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(InVertexFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &VertexShaderBuffer, &ErrorMessage)))
	{
		// 셰이더 컴파일 실패시 오류메시지를 출력합니다.
		if (ErrorMessage)
		{
			OutputShaderErrorMessage(ErrorMessage, InHwnd, InVertexFilename);
		}
		// 컴파일 오류가 아니라면 셰이더 파일을 찾을 수 없는 경우입니다.
		else
		{
			MessageBox(InHwnd, InVertexFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	// 픽셀 쉐이더 코드를 컴파일한다.
	ID3D10Blob* PixelShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(InPixelFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &PixelShaderBuffer, &ErrorMessage)))
	{
		// 셰이더 컴파일 실패시 오류메시지를 출력합니다.
		if (ErrorMessage)
		{
			OutputShaderErrorMessage(ErrorMessage, InHwnd, InPixelFilename);
		}
		// 컴파일 오류가 아니라면 셰이더 파일을 찾을 수 없는 경우입니다.
		else
		{
			MessageBox(InHwnd, InPixelFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}


	// 버퍼로부터 정점 셰이더를 생성
	if (FAILED(InDevice->CreateVertexShader(VertexShaderBuffer->GetBufferPointer(), VertexShaderBuffer->GetBufferSize(), NULL, &m_VertexShader)))
	{
		return false;
	}

	// 버퍼에서 픽셀 쉐이더를 생성
	if (FAILED(InDevice->CreatePixelShader(PixelShaderBuffer->GetBufferPointer(), PixelShaderBuffer->GetBufferSize(), NULL, &m_PixelShader)))
	{
		return false;
	}

	// 정점 입력 레이아웃 구조체를 설정
	// 이 설정은 ModelClass와 셰이더의 VertexType 구조와 일치해야함.
	D3D11_INPUT_ELEMENT_DESC PolygonLayOut[2];
	PolygonLayOut[0].SemanticName = "POSITION";
	PolygonLayOut[0].SemanticIndex = 0;
	PolygonLayOut[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	PolygonLayOut[0].InputSlot = 0;
	PolygonLayOut[0].AlignedByteOffset = 0;
	PolygonLayOut[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	PolygonLayOut[0].InstanceDataStepRate = 0;

	PolygonLayOut[1].SemanticName = "COLOR";
	PolygonLayOut[1].SemanticIndex = 0;
	PolygonLayOut[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	PolygonLayOut[1].InputSlot = 0;
	PolygonLayOut[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	PolygonLayOut[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	PolygonLayOut[1].InstanceDataStepRate = 0;

	// 레이아웃의 요소 수를 가져옴
	unsigned int NumElements = sizeof(PolygonLayOut) / sizeof(PolygonLayOut[0]);

	// 정점 입력 레이아웃을 만듬
	if (FAILED(InDevice->CreateInputLayout(PolygonLayOut, NumElements,
		VertexShaderBuffer->GetBufferPointer(), VertexShaderBuffer->GetBufferSize(), &m_Layout)))
	{
		return false;
	}

	// 더 이상 사용되지 않는 정점 셰이더 퍼버와 픽셀 셰이더 버퍼를 해제합니다.
	VertexShaderBuffer->Release();
	VertexShaderBuffer = nullptr;

	PixelShaderBuffer->Release();
	PixelShaderBuffer = nullptr;

	// 정점 셰이더에 있는 행렬 상수 버퍼의 구조체를 작성합니다.
	D3D11_BUFFER_DESC MatrixBufferDesc;
	MatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	MatrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	MatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	MatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	MatrixBufferDesc.MiscFlags = 0;
	MatrixBufferDesc.StructureByteStride = 0;

	// 상수 버퍼 포인터를 만들어 이 클래스에서 정점 셰이더 상수 버퍼에 접근할 수 있게 합니다.
	if (FAILED(InDevice->CreateBuffer(&MatrixBufferDesc, NULL, &m_MatrixBuffer)))
	{
		return false;

	}
	return true;
}

void ColorShader::Shutdown()
{
	// 버텍스 및 픽셀 쉐이더와 관련된 객체를 종료.
	ShutdownShader();
}

void ColorShader::ShutdownShader()
{
	// 행렬 상수 버퍼를 해제합니다.
	if (m_MatrixBuffer)
	{
		m_MatrixBuffer->Release();
		m_MatrixBuffer = nullptr;
	}

	// 레이아웃을 해제합니다.
	if (m_Layout)
	{
		m_Layout->Release();
		m_Layout = nullptr;
	}

	// 픽셀 쉐이더를 해제합니다.
	if (m_PixelShader)
	{
		m_PixelShader->Release();
		m_PixelShader = nullptr;
	}

	// 버텍스 쉐이더를 해제합니다.
	if (m_VertexShader)
	{
		m_VertexShader->Release();
		m_VertexShader = nullptr;
	}
}



bool ColorShader::SetShaderParameters(ID3D11DeviceContext* IndeviceContext
	, XMMATRIX InWorldMatrix, XMMATRIX InViewMatrix, XMMATRIX InProjectionMatrix)
{
	// 행렬을 Transpose하여 셰이더에서 사용할 수 있게 한다.
	InWorldMatrix = XMMatrixTranspose(InWorldMatrix);
	InViewMatrix = XMMatrixTranspose(InViewMatrix);
	InProjectionMatrix = XMMatrixTranspose(InProjectionMatrix);

	// 상수 버퍼의 내용을 쓸 수 있도록 잠급니다.
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	if (FAILED(IndeviceContext->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource)))
	{
		return false;
	}

	// 상수 버퍼의 데이터에 대한 포인터를 가져옵니다.
	MatrixBufferType* DataPtr = (MatrixBufferType*)MappedResource.pData;
	// 상수 버퍼에 행렬을 복사합니다.
	DataPtr->world = InWorldMatrix;
	DataPtr->view = InViewMatrix;
	DataPtr->projection = InProjectionMatrix;

	// 상수 버퍼의 잠금을 풉니다.
	IndeviceContext->Unmap(m_MatrixBuffer, 0);

	// 정점 셰이더에서의 상수 버퍼의 위치를 설정합니다.
	unsigned bufferNumber = 0;

	// 마지막으로 정점 셰이더의 상수 버퍼를 바뀐 값으로 바꿈
	IndeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_MatrixBuffer);
	return true;
}

bool ColorShader::Render(ID3D11DeviceContext* InDeviceContext, int InIndexCount
	, XMMATRIX InWorldMatrix, XMMATRIX InViewMatrix, XMMATRIX InProjectionMatrix)
{
	// 렌더링에 사용할 셰이더 매개변수 설정
	if (!SetShaderParameters(InDeviceContext, InWorldMatrix, InViewMatrix, InProjectionMatrix))
		return false;

	// 설정된 버퍼를 셰이더로 렌더링
	RenderShader(InDeviceContext, InIndexCount);

	return true;
}

void ColorShader::RenderShader(ID3D11DeviceContext* IndeviceContext, int InIndexCount)
{
	// 정점 입력 레이아웃을 설정합니다.
	IndeviceContext->IASetInputLayout(m_Layout);

	// 삼각형을 그릴 정점 셰이더와 픽셀 셰이더를 설정합니다.
	IndeviceContext->VSSetShader(m_VertexShader, NULL, 0);
	IndeviceContext->PSSetShader(m_PixelShader, NULL, 0);

	// 삼각형을 그립니다.
	IndeviceContext->DrawIndexed(InIndexCount, 0, 0);
}


void ColorShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
	// 에러 메시지를 출력창에 표시합니다.
	OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

	// 에러 메세지를 반환합니다.
	errorMessage->Release();
	errorMessage = 0;

	// 컴파일 에러가 있음을 팝업 메세지로 알려줍니다.
	MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}