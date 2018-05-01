#include "stdafx.h"
#include "TextureShader.h"


TextureShader::TextureShader()
{
}


TextureShader::~TextureShader()
{
}


bool TextureShader::Initialize(ID3D11Device* InDevice, HWND InHwnd)
{
	return InitializeShader(InDevice, InHwnd, L"../DirectX11_FrameWork/Shader/Texture.vs", L"../DirectX11_FrameWork/Shader/Texture.ps");
}

void TextureShader::Shutdown()
{
	ShutdownShader();
}

bool TextureShader::Render(ID3D11DeviceContext* InDeviceContext, int InIndexCount
	, XMMATRIX InWorldMatrix, XMMATRIX InViewMatrix, XMMATRIX InProjectionMatrix, ID3D11ShaderResourceView* InTexture)
{
	if (!SetShaderParameters(InDeviceContext, InWorldMatrix, InViewMatrix, InProjectionMatrix, InTexture))
	{
		return false;
	}

	// 설정된 버퍼를 셰이더로 렌더링한다.
	RenderShader(InDeviceContext, InIndexCount);

	return true;
}

bool TextureShader::InitializeShader(ID3D11Device* InDevice, HWND InHwnd, const WCHAR* InVertexFileName, const WCHAR* InPixelFileName)
{
	HRESULT Result;
	ID3D10Blob* ErrorMessage = nullptr;

	ID3D10Blob* VertexShaderBuffer = nullptr;

	Result = D3DCompileFromFile(InVertexFileName, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, &VertexShaderBuffer, &ErrorMessage);

	if (FAILED(Result))
	{
		if (ErrorMessage)
		{
			OutputShaderErrorMessage(ErrorMessage, InHwnd, InVertexFileName);
		}
		else
		{
			MessageBox(InHwnd, InVertexFileName, L"Missing Shader File", MB_OK);
		}
	}

	// 픽셀 쉐이더 코드를 컴파일한다.
	ID3D10Blob* PixelShaderBuffer = nullptr;
	Result = D3DCompileFromFile(InPixelFileName, nullptr, nullptr, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &PixelShaderBuffer, &ErrorMessage);

	if (FAILED(Result))
	{
		if (ErrorMessage)
		{
			OutputShaderErrorMessage(ErrorMessage, InHwnd, InPixelFileName);
		}
		else
		{
			MessageBox(InHwnd, InPixelFileName, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// 버퍼로부터 정점 셰이더를 생성
	Result = InDevice->CreateVertexShader(VertexShaderBuffer->GetBufferPointer(), VertexShaderBuffer->GetBufferSize(), nullptr, &m_VertexShader);

	if (FAILED(Result))
	{
		return false;
	}

	Result = InDevice->CreatePixelShader(PixelShaderBuffer->GetBufferPointer(), PixelShaderBuffer->GetBufferSize(), nullptr, &m_PixelShader);

	if (FAILED(Result))
	{
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC PolygonLayout[2];
	PolygonLayout[0].SemanticName = "POSITION";
	PolygonLayout[0].SemanticIndex = 0;
	PolygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	PolygonLayout[0].InputSlot = 0;
	PolygonLayout[0].AlignedByteOffset = 0;
	PolygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	PolygonLayout[0].InstanceDataStepRate = 0;

	PolygonLayout[1].SemanticName = "TEXCOORD";
	PolygonLayout[1].SemanticIndex = 0;
	PolygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	PolygonLayout[1].InputSlot = 0;
	PolygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	PolygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	PolygonLayout[1].InstanceDataStepRate = 0;

	// 레이아웃의 요소 수를 가져옵니다.
	UINT NumElements = sizeof(PolygonLayout) / sizeof(PolygonLayout[0]);

	// 정점 입력 레이아웃을 만듭니다.
	Result = InDevice->CreateInputLayout(PolygonLayout, NumElements, VertexShaderBuffer->GetBufferPointer(), VertexShaderBuffer->GetBufferSize(), &m_Layout);

	if (FAILED(Result))
	{
		return false;
	}

	// 버퍼 해제
	VertexShaderBuffer->Release();
	VertexShaderBuffer = nullptr;

	PixelShaderBuffer->Release();
	PixelShaderBuffer = nullptr;

	// 정점 셰이더에 있는 행렬 상수 버퍼의 구조체를 작성
	D3D11_BUFFER_DESC MatrixBufferDesc;
	MatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	MatrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	MatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	MatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	MatrixBufferDesc.MiscFlags = 0;
	MatrixBufferDesc.StructureByteStride = 0;

	// 상수 버퍼 포인터를 만들어 이 클래스에서 정점 셰이더 상수 버퍼에 접근할 수 있게 합니다.
	Result = InDevice->CreateBuffer(&MatrixBufferDesc, nullptr, &m_MatrixBuffer);
	if (FAILED(Result))
	{
		return false;
	}

	// 텍스처 샘플러 상태 구조체를 생성 및 설정합니다.
	D3D11_SAMPLER_DESC SamplerDesc;
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.MipLODBias = 0.0f;
	SamplerDesc.MaxAnisotropy = 1;
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	SamplerDesc.BorderColor[0] = 0;
	SamplerDesc.BorderColor[1] = 0;
	SamplerDesc.BorderColor[2] = 0;
	SamplerDesc.BorderColor[3] = 0;
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD =  D3D11_FLOAT32_MAX;

	// 텍스처 샘플러 상태를 만듭니다.
	Result = InDevice->CreateSamplerState(&SamplerDesc, &m_SampleState);
	if (FAILED(Result))
	{
		return false;
	}

	return true;
}

void TextureShader::ShutdownShader()
{
	if (m_SampleState)
	{
		m_SampleState->Release();
		m_SampleState = nullptr;
	}

	if (m_MatrixBuffer)
	{
		m_MatrixBuffer->Release();
		m_MatrixBuffer = nullptr;
	}

	if (m_Layout)
	{
		m_Layout->Release();
		m_Layout = nullptr;
	}

	if (m_PixelShader)
	{
		m_PixelShader->Release();
		m_PixelShader = nullptr;
	}

	if (m_VertexShader)
	{
		m_VertexShader->Release();
		m_VertexShader = nullptr;
	}
}

void TextureShader::OutputShaderErrorMessage(ID3D10Blob* InErrorMesage, HWND InHwnd, const WCHAR* InShaderFileName)
{
	OutputDebugString(reinterpret_cast<const wchar_t*>(InErrorMesage->GetBufferPointer()));

	InErrorMesage->Release();
	InErrorMesage = nullptr;

	MessageBox(InHwnd, L"Error compiling Shader", InShaderFileName, MB_OK);
}

bool TextureShader::SetShaderParameters(ID3D11DeviceContext* InDeviceContext, XMMATRIX InWorldMatrix, XMMATRIX InViewMatrix, XMMATRIX InProjectionMatrix, ID3D11ShaderResourceView* InTexture)
{
	// 행렬을 셰이더에서 사용할 수 있게 합니다.
	InWorldMatrix = XMMatrixTranspose(InWorldMatrix);
	InViewMatrix = XMMatrixTranspose(InViewMatrix);
	InProjectionMatrix = XMMatrixTranspose(InProjectionMatrix);

	// 상수 버퍼의 내용을 쓸 수 있도록 잠급니다.
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	if (FAILED(InDeviceContext->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource)))
	{
		return false;
	}

	// 상수 버퍼의 데이터에 대한 포인터를 가져옵니다.
	MatrixBufferType* DataPtr = (MatrixBufferType*)MappedResource.pData;

	// 상수 버퍼에 행렬을 복사합니다.
	DataPtr->world = InWorldMatrix;
	DataPtr->view = InViewMatrix;
	DataPtr->projection = InProjectionMatrix;

	InDeviceContext->Unmap(m_MatrixBuffer, 0);

	// 정점 셰이더에서의 상수 버퍼의 위치를 설정합니다.
	unsigned int BufferNumber = 0;

	// 마지막으로 정점 셰이더의 상수 버퍼를 바뀐 값으로 바꿉니다.
	InDeviceContext->VSSetConstantBuffers(BufferNumber, 1, &m_MatrixBuffer);

	// 픽셀 셰이더에서 셰이더 텍스처 리소스를 설정합니다.
	InDeviceContext->PSSetShaderResources(0, 1, &InTexture);
	return true;
}

void TextureShader::RenderShader(ID3D11DeviceContext* InDeviceContext, int InIndexCount)
{

	// 정점 입력 레이아웃 설정
	InDeviceContext->IASetInputLayout(m_Layout);

	// 삼각형을 그릴 셰이더 설정
	InDeviceContext->VSSetShader(m_VertexShader, nullptr, 0);
	InDeviceContext->PSSetShader(m_PixelShader, nullptr, 0);

	// 픽셀 셰이더에서 샘플러 상태를 설정합니다.
	InDeviceContext->PSSetSamplers(0, 1, &m_SampleState);

	// 삼각형을 그립니다.
	InDeviceContext->DrawIndexed(InIndexCount, 0, 0);
}