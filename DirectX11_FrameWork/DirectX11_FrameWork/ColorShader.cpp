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
	// ���� �� �ȼ� ���̴��� �ʱ�ȭ.
	return InitializeShader(InDevice, InHwnd, L"../DirectX11_FrameWork/color.vs", L"../DirectX11_FrameWork/color.ps");
}

bool ColorShader::InitializeShader(ID3D11Device* InDevice, HWND InHwnd, const WCHAR* InVertexFilename, const WCHAR* InPixelFilename)
{
	ID3D10Blob* ErrorMessage = nullptr;

	// ���ؽ� ���̴� �ڵ带 ������
	ID3D10Blob* VertexShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(InVertexFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &VertexShaderBuffer, &ErrorMessage)))
	{
		// ���̴� ������ ���н� �����޽����� ����մϴ�.
		if (ErrorMessage)
		{
			OutputShaderErrorMessage(ErrorMessage, InHwnd, InVertexFilename);
		}
		// ������ ������ �ƴ϶�� ���̴� ������ ã�� �� ���� ����Դϴ�.
		else
		{
			MessageBox(InHwnd, InVertexFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	// �ȼ� ���̴� �ڵ带 �������Ѵ�.
	ID3D10Blob* PixelShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(InPixelFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &PixelShaderBuffer, &ErrorMessage)))
	{
		// ���̴� ������ ���н� �����޽����� ����մϴ�.
		if (ErrorMessage)
		{
			OutputShaderErrorMessage(ErrorMessage, InHwnd, InPixelFilename);
		}
		// ������ ������ �ƴ϶�� ���̴� ������ ã�� �� ���� ����Դϴ�.
		else
		{
			MessageBox(InHwnd, InPixelFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}


	// ���۷κ��� ���� ���̴��� ����
	if (FAILED(InDevice->CreateVertexShader(VertexShaderBuffer->GetBufferPointer(), VertexShaderBuffer->GetBufferSize(), NULL, &m_VertexShader)))
	{
		return false;
	}

	// ���ۿ��� �ȼ� ���̴��� ����
	if (FAILED(InDevice->CreatePixelShader(PixelShaderBuffer->GetBufferPointer(), PixelShaderBuffer->GetBufferSize(), NULL, &m_PixelShader)))
	{
		return false;
	}

	// ���� �Է� ���̾ƿ� ����ü�� ����
	// �� ������ ModelClass�� ���̴��� VertexType ������ ��ġ�ؾ���.
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

	// ���̾ƿ��� ��� ���� ������
	unsigned int NumElements = sizeof(PolygonLayOut) / sizeof(PolygonLayOut[0]);

	// ���� �Է� ���̾ƿ��� ����
	if (FAILED(InDevice->CreateInputLayout(PolygonLayOut, NumElements,
		VertexShaderBuffer->GetBufferPointer(), VertexShaderBuffer->GetBufferSize(), &m_Layout)))
	{
		return false;
	}

	// �� �̻� ������ �ʴ� ���� ���̴� �۹��� �ȼ� ���̴� ���۸� �����մϴ�.
	VertexShaderBuffer->Release();
	VertexShaderBuffer = nullptr;

	PixelShaderBuffer->Release();
	PixelShaderBuffer = nullptr;

	// ���� ���̴��� �ִ� ��� ��� ������ ����ü�� �ۼ��մϴ�.
	D3D11_BUFFER_DESC MatrixBufferDesc;
	MatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	MatrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	MatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	MatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	MatrixBufferDesc.MiscFlags = 0;
	MatrixBufferDesc.StructureByteStride = 0;

	// ��� ���� �����͸� ����� �� Ŭ�������� ���� ���̴� ��� ���ۿ� ������ �� �ְ� �մϴ�.
	if (FAILED(InDevice->CreateBuffer(&MatrixBufferDesc, NULL, &m_MatrixBuffer)))
	{
		return false;

	}
	return true;
}

void ColorShader::Shutdown()
{
	// ���ؽ� �� �ȼ� ���̴��� ���õ� ��ü�� ����.
	ShutdownShader();
}

void ColorShader::ShutdownShader()
{
	// ��� ��� ���۸� �����մϴ�.
	if (m_MatrixBuffer)
	{
		m_MatrixBuffer->Release();
		m_MatrixBuffer = nullptr;
	}

	// ���̾ƿ��� �����մϴ�.
	if (m_Layout)
	{
		m_Layout->Release();
		m_Layout = nullptr;
	}

	// �ȼ� ���̴��� �����մϴ�.
	if (m_PixelShader)
	{
		m_PixelShader->Release();
		m_PixelShader = nullptr;
	}

	// ���ؽ� ���̴��� �����մϴ�.
	if (m_VertexShader)
	{
		m_VertexShader->Release();
		m_VertexShader = nullptr;
	}
}



bool ColorShader::SetShaderParameters(ID3D11DeviceContext* IndeviceContext
	, XMMATRIX InWorldMatrix, XMMATRIX InViewMatrix, XMMATRIX InProjectionMatrix)
{
	// ����� Transpose�Ͽ� ���̴����� ����� �� �ְ� �Ѵ�.
	InWorldMatrix = XMMatrixTranspose(InWorldMatrix);
	InViewMatrix = XMMatrixTranspose(InViewMatrix);
	InProjectionMatrix = XMMatrixTranspose(InProjectionMatrix);

	// ��� ������ ������ �� �� �ֵ��� ��޴ϴ�.
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	if (FAILED(IndeviceContext->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource)))
	{
		return false;
	}

	// ��� ������ �����Ϳ� ���� �����͸� �����ɴϴ�.
	MatrixBufferType* DataPtr = (MatrixBufferType*)MappedResource.pData;
	// ��� ���ۿ� ����� �����մϴ�.
	DataPtr->world = InWorldMatrix;
	DataPtr->view = InViewMatrix;
	DataPtr->projection = InProjectionMatrix;

	// ��� ������ ����� Ǳ�ϴ�.
	IndeviceContext->Unmap(m_MatrixBuffer, 0);

	// ���� ���̴������� ��� ������ ��ġ�� �����մϴ�.
	unsigned bufferNumber = 0;

	// ���������� ���� ���̴��� ��� ���۸� �ٲ� ������ �ٲ�
	IndeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_MatrixBuffer);
	return true;
}

bool ColorShader::Render(ID3D11DeviceContext* InDeviceContext, int InIndexCount
	, XMMATRIX InWorldMatrix, XMMATRIX InViewMatrix, XMMATRIX InProjectionMatrix)
{
	// �������� ����� ���̴� �Ű����� ����
	if (!SetShaderParameters(InDeviceContext, InWorldMatrix, InViewMatrix, InProjectionMatrix))
		return false;

	// ������ ���۸� ���̴��� ������
	RenderShader(InDeviceContext, InIndexCount);

	return true;
}

void ColorShader::RenderShader(ID3D11DeviceContext* IndeviceContext, int InIndexCount)
{
	// ���� �Է� ���̾ƿ��� �����մϴ�.
	IndeviceContext->IASetInputLayout(m_Layout);

	// �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����մϴ�.
	IndeviceContext->VSSetShader(m_VertexShader, NULL, 0);
	IndeviceContext->PSSetShader(m_PixelShader, NULL, 0);

	// �ﰢ���� �׸��ϴ�.
	IndeviceContext->DrawIndexed(InIndexCount, 0, 0);
}


void ColorShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
	// ���� �޽����� ���â�� ǥ���մϴ�.
	OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

	// ���� �޼����� ��ȯ�մϴ�.
	errorMessage->Release();
	errorMessage = 0;

	// ������ ������ ������ �˾� �޼����� �˷��ݴϴ�.
	MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}