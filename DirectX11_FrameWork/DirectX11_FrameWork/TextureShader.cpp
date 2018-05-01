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

	// ������ ���۸� ���̴��� �������Ѵ�.
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

	// �ȼ� ���̴� �ڵ带 �������Ѵ�.
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

	// ���۷κ��� ���� ���̴��� ����
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

	// ���̾ƿ��� ��� ���� �����ɴϴ�.
	UINT NumElements = sizeof(PolygonLayout) / sizeof(PolygonLayout[0]);

	// ���� �Է� ���̾ƿ��� ����ϴ�.
	Result = InDevice->CreateInputLayout(PolygonLayout, NumElements, VertexShaderBuffer->GetBufferPointer(), VertexShaderBuffer->GetBufferSize(), &m_Layout);

	if (FAILED(Result))
	{
		return false;
	}

	// ���� ����
	VertexShaderBuffer->Release();
	VertexShaderBuffer = nullptr;

	PixelShaderBuffer->Release();
	PixelShaderBuffer = nullptr;

	// ���� ���̴��� �ִ� ��� ��� ������ ����ü�� �ۼ�
	D3D11_BUFFER_DESC MatrixBufferDesc;
	MatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	MatrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	MatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	MatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	MatrixBufferDesc.MiscFlags = 0;
	MatrixBufferDesc.StructureByteStride = 0;

	// ��� ���� �����͸� ����� �� Ŭ�������� ���� ���̴� ��� ���ۿ� ������ �� �ְ� �մϴ�.
	Result = InDevice->CreateBuffer(&MatrixBufferDesc, nullptr, &m_MatrixBuffer);
	if (FAILED(Result))
	{
		return false;
	}

	// �ؽ�ó ���÷� ���� ����ü�� ���� �� �����մϴ�.
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

	// �ؽ�ó ���÷� ���¸� ����ϴ�.
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
	// ����� ���̴����� ����� �� �ְ� �մϴ�.
	InWorldMatrix = XMMatrixTranspose(InWorldMatrix);
	InViewMatrix = XMMatrixTranspose(InViewMatrix);
	InProjectionMatrix = XMMatrixTranspose(InProjectionMatrix);

	// ��� ������ ������ �� �� �ֵ��� ��޴ϴ�.
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	if (FAILED(InDeviceContext->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource)))
	{
		return false;
	}

	// ��� ������ �����Ϳ� ���� �����͸� �����ɴϴ�.
	MatrixBufferType* DataPtr = (MatrixBufferType*)MappedResource.pData;

	// ��� ���ۿ� ����� �����մϴ�.
	DataPtr->world = InWorldMatrix;
	DataPtr->view = InViewMatrix;
	DataPtr->projection = InProjectionMatrix;

	InDeviceContext->Unmap(m_MatrixBuffer, 0);

	// ���� ���̴������� ��� ������ ��ġ�� �����մϴ�.
	unsigned int BufferNumber = 0;

	// ���������� ���� ���̴��� ��� ���۸� �ٲ� ������ �ٲߴϴ�.
	InDeviceContext->VSSetConstantBuffers(BufferNumber, 1, &m_MatrixBuffer);

	// �ȼ� ���̴����� ���̴� �ؽ�ó ���ҽ��� �����մϴ�.
	InDeviceContext->PSSetShaderResources(0, 1, &InTexture);
	return true;
}

void TextureShader::RenderShader(ID3D11DeviceContext* InDeviceContext, int InIndexCount)
{

	// ���� �Է� ���̾ƿ� ����
	InDeviceContext->IASetInputLayout(m_Layout);

	// �ﰢ���� �׸� ���̴� ����
	InDeviceContext->VSSetShader(m_VertexShader, nullptr, 0);
	InDeviceContext->PSSetShader(m_PixelShader, nullptr, 0);

	// �ȼ� ���̴����� ���÷� ���¸� �����մϴ�.
	InDeviceContext->PSSetSamplers(0, 1, &m_SampleState);

	// �ﰢ���� �׸��ϴ�.
	InDeviceContext->DrawIndexed(InIndexCount, 0, 0);
}