#include "stdafx.h"
#include "modelclass.h"
#include "BaseTexture.h"

#include <fstream>
using namespace std;

ModelClass::ModelClass()
{
}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}


bool ModelClass::Initialize(ID3D11Device* InDevice, const WCHAR* InModelFileName, const WCHAR* InTextureFileName)
{
	if (!LoadModel(InModelFileName))
	{
		return false;
	}
	// ���� �� �ε��� ���۸� �ʱ�ȭ
	if (!InitializeBuffers(InDevice))
		return false;

	// �� ���� �ؽ�ó �ε�
	return LoadTexture(InDevice, InTextureFileName);
}

bool ModelClass::LoadModel(const WCHAR* filename)
{
	// �� ������ ���ϴ�.
	ifstream fin;
	fin.open(filename);

	// ������ �� �� ������ ����
	if (fin.fail())
	{
		return false;
	}

	// ���ؽ� ī��Ʈ�� ������ �д´�.
	char input = 0;
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	//���ؽ� ī��Ʈ�� ����
	fin >> m_VertexCount;

	// �ε����� ���� ���� ���� ���� �����Ѵ�.
	m_IndexCount = m_VertexCount;

	m_model = new ModelType[m_VertexCount];
	if (!m_model)
	{
		return false;
	}
	// �������� ���� �κб��� �д´�.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// ���ؽ� �����͸� �н��ϴ�.
	for (int i = 0; i < m_VertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// �� ������ �ݴ´�.
	fin.close();

	return true;
}

void ModelClass::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}
}
void ModelClass::Shutdown()
{
	// ���� �ؽ�ó ��ȯ
	ReleaseTexture();

	// ���ؽ� �� �ε��� ���۸� ����
	ShutdownBuffers();

	ReleaseModel();
}


ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
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

bool ModelClass::InitializeBuffers(ID3D11Device* InDevice)
{
	VertexType* Vertices = new VertexType[m_VertexCount];
	if (!Vertices)
	{
		return false;
	}

	// �ε��� �迭�� ����ϴ�.
	unsigned long* Indices = new unsigned long[m_IndexCount];
	if (!Indices)
	{
		return false;
	}

	// ���� �迭�� �ε��� �迭�� �����ͷ� �о�ɴϴ�.
	for (int i = 0; i < m_VertexCount; ++i)
	{
		Vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		Vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		Vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		Indices[i] = i;
	}

	// ���� ���� ������ ����ü�� �����մϴ�.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_VertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource ������ ���� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = Vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���� ���۸� ����ϴ�.
	if (FAILED(InDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer)))
	{
		return false;
	}

	// ���� �ε��� ������ ����ü�� �����մϴ�.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_IndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// �ε��� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �ۼ��մϴ�.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = Indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;


	// �ε��� ���۸� �����մϴ�.
	if (FAILED(InDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer)))
	{
		return false;
	}

	// �����ǰ� ���� �Ҵ�� ���� ���ۿ� �ε��� ���۸� �����մϴ�.
	delete[] Vertices;
	Vertices = 0;

	delete[] Indices;
	Indices = 0;

	return true;
}
// 7�� ������
//bool ModelClass::InitializeBuffers(ID3D11Device* device)
//{	
//
//	// ���� �迭�� ���� ���� ����
//	m_VertexCount = 3;
//	m_IndexCount = 3; // ���Ͽ��� �̸� �о ����ϰ�.
//
//	VertexType* Vertices = new VertexType[m_VertexCount];
//	if (!m_IndexCount)
//		return false;
//
//	// �ε��� �迭
//	unsigned long* Indices = new unsigned long[m_IndexCount];
//	if (!Indices)
//		return false;
//
//	// ���� �迭�� ���� �����մϴ�.
//	Vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
//	Vertices[0].texture = XMFLOAT2(0.0f, 1.0f);
//
//	Vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);  // Top middle.
//	Vertices[1].texture = XMFLOAT2(0.5f, 0.0f);
//
//	Vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
//	Vertices[2].texture = XMFLOAT2(1.0f, 1.0f);
//
//	// �ε��� �迭�� ���� ����
//	Indices[0] = 0;  // Bottom left.
//	Indices[1] = 1;  // Top middle.
//	Indices[2] = 2;  // Bottom right.
//
//	// ���� ���� ������ ����ü ����
//	D3D11_BUFFER_DESC VertexBufferDesc;
//	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	VertexBufferDesc.ByteWidth = sizeof(VertexType) * m_VertexCount;
//	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	VertexBufferDesc.CPUAccessFlags = 0;
//	VertexBufferDesc.MiscFlags = 0;
//	VertexBufferDesc.StructureByteStride = 0;
//
//	// subresource ������ ���� �����Ϳ� ���� �����͸� �����մϴ�.
//	D3D11_SUBRESOURCE_DATA VertexData;
//	VertexData.pSysMem = Vertices;
//	VertexData.SysMemPitch = 0;
//	VertexData.SysMemSlicePitch = 0;
//	
//	// ���� ���۸� ����
//	// ���� ���� ���۸� ����ϴ�.
//	if (FAILED(device->CreateBuffer(&VertexBufferDesc, &VertexData, &m_VertexBuffer)))
//	{
//		return false;
//	}
//
//	// ���� �ε��� ������ ����ü�� ����.
//	D3D11_BUFFER_DESC IndexBufferDesc;
//	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	IndexBufferDesc.ByteWidth = sizeof(unsigned long) * m_IndexCount;
//	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	IndexBufferDesc.CPUAccessFlags = 0;
//	IndexBufferDesc.MiscFlags = 0;
//	IndexBufferDesc.StructureByteStride = 0;
//
//	// �ε��� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �ۼ�.
//	D3D11_SUBRESOURCE_DATA IndexData;
//	IndexData.pSysMem = Indices;
//	IndexData.SysMemPitch = 0;
//	IndexData.SysMemSlicePitch = 0;
//
//	// �ε��� ���۸� �����մϴ�.
//	if (FAILED(device->CreateBuffer(&IndexBufferDesc, &IndexData, &m_IndexBuffer)))
//	{
//		return false;
//	}
//
//	// �����ǰ� ���� �Ҵ�� ���� ���ۿ� �ε��� ���۸� �����մϴ�.
//	delete[] Vertices;
//	Vertices = nullptr;
//
//	delete[] Indices;
//	Indices = nullptr;
//
//	return true;
//}


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

bool ModelClass::LoadTexture(ID3D11Device* InDevice, const WCHAR* InFileName)
{
	m_Texture = new BaseTexture;
	if (!m_Texture)
	{
		return false;
	}

	return m_Texture->Initialize(InDevice, InFileName);
}

void ModelClass::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = nullptr;
	}
}