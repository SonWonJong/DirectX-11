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
	// 정점 및 인덱스 버퍼를 초기화
	if (!InitializeBuffers(InDevice))
		return false;

	// 이 모델의 텍스처 로드
	return LoadTexture(InDevice, InTextureFileName);
}

bool ModelClass::LoadModel(const WCHAR* filename)
{
	// 모델 파일을 엽니다.
	ifstream fin;
	fin.open(filename);

	// 파일을 열 수 없으면 종료
	if (fin.fail())
	{
		return false;
	}

	// 버텍스 카운트의 값까지 읽는다.
	char input = 0;
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	//버텍스 카운트를 읽음
	fin >> m_VertexCount;

	// 인덱스의 수를 정점 수와 같게 설정한다.
	m_IndexCount = m_VertexCount;

	m_model = new ModelType[m_VertexCount];
	if (!m_model)
	{
		return false;
	}
	// 데이터의 시작 부분까지 읽는다.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// 버텍스 데이터를 읽습니다.
	for (int i = 0; i < m_VertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// 모델 파일을 닫는다.
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
	// 모델의 텍스처 반환
	ReleaseTexture();

	// 버텍스 및 인덱스 버퍼를 종료
	ShutdownBuffers();

	ReleaseModel();
}


ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// 그리기를 준비하기 위해 그래픽 파이프 라인에 꼭지점과 인덱스 버퍼를 놓습니다.
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

	// 인덱스 배열을 만듭니다.
	unsigned long* Indices = new unsigned long[m_IndexCount];
	if (!Indices)
	{
		return false;
	}

	// 정점 배열과 인덱스 배열을 데이터로 읽어옵니다.
	for (int i = 0; i < m_VertexCount; ++i)
	{
		Vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		Vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		Vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		Indices[i] = i;
	}

	// 정적 정점 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_VertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = Vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 이제 정점 버퍼를 만듭니다.
	if (FAILED(InDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer)))
	{
		return false;
	}

	// 정적 인덱스 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_IndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 데이터를 가리키는 보조 리소스 구조체를 작성합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = Indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;


	// 인덱스 버퍼를 생성합니다.
	if (FAILED(InDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer)))
	{
		return false;
	}

	// 생성되고 값이 할당된 정점 버퍼와 인덱스 버퍼를 해제합니다.
	delete[] Vertices;
	Vertices = 0;

	delete[] Indices;
	Indices = 0;

	return true;
}
// 7장 전까지
//bool ModelClass::InitializeBuffers(ID3D11Device* device)
//{	
//
//	// 정점 배열의 정점 수를 설정
//	m_VertexCount = 3;
//	m_IndexCount = 3; // 파일에서 미리 읽어서 계산하고.
//
//	VertexType* Vertices = new VertexType[m_VertexCount];
//	if (!m_IndexCount)
//		return false;
//
//	// 인덱스 배열
//	unsigned long* Indices = new unsigned long[m_IndexCount];
//	if (!Indices)
//		return false;
//
//	// 정점 배열에 값을 설정합니다.
//	Vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
//	Vertices[0].texture = XMFLOAT2(0.0f, 1.0f);
//
//	Vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);  // Top middle.
//	Vertices[1].texture = XMFLOAT2(0.5f, 0.0f);
//
//	Vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
//	Vertices[2].texture = XMFLOAT2(1.0f, 1.0f);
//
//	// 인덱스 배열의 값을 설정
//	Indices[0] = 0;  // Bottom left.
//	Indices[1] = 1;  // Top middle.
//	Indices[2] = 2;  // Bottom right.
//
//	// 정적 정점 버퍼의 구조체 설정
//	D3D11_BUFFER_DESC VertexBufferDesc;
//	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	VertexBufferDesc.ByteWidth = sizeof(VertexType) * m_VertexCount;
//	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	VertexBufferDesc.CPUAccessFlags = 0;
//	VertexBufferDesc.MiscFlags = 0;
//	VertexBufferDesc.StructureByteStride = 0;
//
//	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
//	D3D11_SUBRESOURCE_DATA VertexData;
//	VertexData.pSysMem = Vertices;
//	VertexData.SysMemPitch = 0;
//	VertexData.SysMemSlicePitch = 0;
//	
//	// 정점 버퍼를 만듬
//	// 이제 정점 버퍼를 만듭니다.
//	if (FAILED(device->CreateBuffer(&VertexBufferDesc, &VertexData, &m_VertexBuffer)))
//	{
//		return false;
//	}
//
//	// 정적 인덱스 버퍼의 구조체를 설정.
//	D3D11_BUFFER_DESC IndexBufferDesc;
//	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	IndexBufferDesc.ByteWidth = sizeof(unsigned long) * m_IndexCount;
//	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	IndexBufferDesc.CPUAccessFlags = 0;
//	IndexBufferDesc.MiscFlags = 0;
//	IndexBufferDesc.StructureByteStride = 0;
//
//	// 인덱스 데이터를 가리키는 보조 리소스 구조체를 작성.
//	D3D11_SUBRESOURCE_DATA IndexData;
//	IndexData.pSysMem = Indices;
//	IndexData.SysMemPitch = 0;
//	IndexData.SysMemSlicePitch = 0;
//
//	// 인덱스 버퍼를 생성합니다.
//	if (FAILED(device->CreateBuffer(&IndexBufferDesc, &IndexData, &m_IndexBuffer)))
//	{
//		return false;
//	}
//
//	// 생성되고 값이 할당된 정점 버퍼와 인덱스 버퍼를 해제합니다.
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
	// 인덱스 버퍼를 해제합니다.
	if(m_IndexBuffer)
	{
		m_IndexBuffer->Release();
		m_IndexBuffer = nullptr;
	}

	// 정점 버퍼를 해제합니다.
	if(m_VertexBuffer)
	{
		m_VertexBuffer->Release();
		m_VertexBuffer = nullptr;
	}
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// 정점 버퍼의 단위와 오프셋을 설정합니다.
	unsigned int Stride = sizeof(VertexType); 
	unsigned int Offset = 0;
    
	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &Stride, &Offset);

    // 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // 정점 버퍼로 그릴 기본형을 설정합니다. 여기서는 삼각형으로 설정합니다.
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