#include "stdafx.h"
#include "modelclass.h"


ModelClass::ModelClass()
{
}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}


bool ModelClass::Initialize(ID3D11Device* device)
{
	// ���� �� �ε��� ���۸� �ʱ�ȭ�մϴ�.
	return InitializeBuffers(device);
}


void ModelClass::Shutdown()
{
	// ���ؽ� �� �ε��� ���۸� �����մϴ�.
	ShutdownBuffers();
}


void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// �׸��⸦ �غ��ϱ� ���� �׷��� ������ ���ο� �������� �ε��� ���۸� �����ϴ�.
	RenderBuffers(deviceContext);
}


int ModelClass::GetIndexCount()
{
	return m_IndexCount;
}


bool ModelClass::InitializeBuffers(ID3D11Device* device)
{	

	// ���� �迭�� ���� ���� ����
	m_VertexCount = 3;
	m_IndexCount = 3; // ���Ͽ��� �̸� �о ����ϰ�.

	VertexType* Vertices = new VertexType[m_VertexCount];
	if (!m_IndexCount)
		return false;

	// �ε��� �迭
	unsigned long* Indices = new unsigned long[m_IndexCount];
	if (!Indices)
		return false;

	// ���� �迭�� �����͸� ����
	Vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	Vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	Vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);  // Top middle.
	Vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	Vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
	Vertices[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	// �ε��� �迭�� ���� ����
	Indices[0] = 0;  // Bottom left.
	Indices[1] = 1;  // Top middle.
	Indices[2] = 2;  // Bottom right.

	// ���� ���� ������ ����ü ����
	D3D11_BUFFER_DESC VertexBufferDesc;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.ByteWidth = sizeof(VertexType) * m_VertexCount;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = 0;
	VertexBufferDesc.MiscFlags = 0;
	VertexBufferDesc.StructureByteStride = 0;

	// subresource ������ ���� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA VertexData;
	VertexData.pSysMem = Vertices;
	VertexData.SysMemPitch = 0;
	VertexData.SysMemSlicePitch = 0;
	
	// ���� ���۸� ����
	// ���� ���� ���۸� ����ϴ�.
	if (FAILED(device->CreateBuffer(&VertexBufferDesc, &VertexData, &m_VertexBuffer)))
	{
		return false;
	}

	// ���� �ε��� ������ ����ü�� ����.
	D3D11_BUFFER_DESC IndexBufferDesc;
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.ByteWidth = sizeof(unsigned long) * m_IndexCount;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;
	IndexBufferDesc.StructureByteStride = 0;

	// �ε��� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �ۼ�.
	D3D11_SUBRESOURCE_DATA IndexData;
	IndexData.pSysMem = Indices;
	IndexData.SysMemPitch = 0;
	IndexData.SysMemSlicePitch = 0;

	// �ε��� ���۸� �����մϴ�.
	if (FAILED(device->CreateBuffer(&IndexBufferDesc, &IndexData, &m_IndexBuffer)))
	{
		return false;
	}

	// �����ǰ� ���� �Ҵ�� ���� ���ۿ� �ε��� ���۸� �����մϴ�.
	delete[] Vertices;
	Vertices = nullptr;

	delete[] Indices;
	Indices = nullptr;

	return true;
}


void ModelClass::ShutdownBuffers()
{
	// �ε��� ���۸� �����մϴ�.
	if(m_IndexBuffer)
	{
		m_IndexBuffer->Release();
		m_IndexBuffer = nullptr;
	}

	// ���� ���۸� �����մϴ�.
	if(m_VertexBuffer)
	{
		m_VertexBuffer->Release();
		m_VertexBuffer = nullptr;
	}
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// ���� ������ ������ �������� �����մϴ�.
	unsigned int Stride = sizeof(VertexType); 
	unsigned int Offset = 0;
    
	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &Stride, &Offset);

    // ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // ���� ���۷� �׸� �⺻���� �����մϴ�. ���⼭�� �ﰢ������ �����մϴ�.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}